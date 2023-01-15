#include "postprocessor.h"
#include "extendable_buffer.h"
#include "exception.h"
#include "ir_ssbo_format.h"
#include "stack.h"

SizedGLintBuffer::~SizedGLintBuffer() {
    delete[] data;
}

const GLuint tokenise_ir_token(const char* token, ParseTree& ir_tokens) {
    // TODO: make this a regualr hashmap. There's a lot of places where ParseTrees should be hashmaps.
    auto ret = ir_tokens.exec(to_uint_string(token));
    if (ret == 0) throw Exception(ExceptionType::Postprocessor, "Could not find IR token \"" + std::string(token) + "\"."); 
    return ret;
}

SizedGLintBuffer postprocess(const GLint* shader_out, GLuint shader_out_size, 
    VariableRegistry& var_reg, ParseTree& ir_tokens, std::string& source) {

    auto buffer = ExtendableBuffer<GLint>(1024);

    // TODO: execute these earlier or better yet, precompute them.
    auto SCOPE_START = tokenise_ir_token("scope_start", ir_tokens);
    auto SCOPE_END = tokenise_ir_token("scope_end", ir_tokens);
    auto LOADABLE = tokenise_ir_token("loadable", ir_tokens);
    auto BREAK = tokenise_ir_token("BREAK", ir_tokens);
    auto CONTINUE = tokenise_ir_token("CONTINUE", ir_tokens);
    auto BREAKABLE = tokenise_ir_token("breakable", ir_tokens);
    auto CONTINUABLE = tokenise_ir_token("continuable", ir_tokens);
    auto JMP = tokenise_ir_token("JMP", ir_tokens);
    auto GOTOABLE = tokenise_ir_token("gotoable", ir_tokens);
    auto GOTO_REPLACE = tokenise_ir_token("goto_replace", ir_tokens);
    auto REPLACE_ME = tokenise_ir_token("replace_me", ir_tokens);
    auto REPLACE_WITH = tokenise_ir_token("replace_with", ir_tokens);

    struct continue_ {
        GLint index;
        GLint scope;
    };

    Stack<GLint> scope_stack;
    Stack<continue_> continue_stack;
    Stack<GLint> break_stack;
    Stack<GLint> replace_stack;
    std::unordered_map<std::string, GLint> labels;
    std::unordered_map<std::string, std::vector<GLint>> unresolved_labels;

    for (u32 i = 0; i < shader_out_size; i++) {
        auto value = shader_out[i];
        if (value == IR_SOURCE_POS_REF || value == IR_REFERNCE) {
            buffer.append(value);
            buffer.append(shader_out[++i]);
            continue;
        }
        if (value == REPLACE_WITH) {
            replace_stack.push(shader_out[i + 2]);
            i += 3;
            continue;
        }
        if (value == REPLACE_ME) {
            if (replace_stack.size() == 0) throw Exception(ExceptionType::Postprocessor, "Malformed IR. `replace_me` token with no `replace_with` token.");
            buffer.append(IR_REFERNCE);
            buffer.append(replace_stack.pop());
            continue;
        }
        if (value == GOTOABLE) {
            // TODO: check how gotos and labels work specifically.
            //       I'm not sure how they work scope wise or whatever.
            if (i + 2 >= shader_out_size) throw Exception(ExceptionType::Postprocessor, "Malformed IR. `gotoable` token at end of shader.");
            // Structure: GOTOABLE $0 $x :
            auto pos = (u32)shader_out[i + 2];
            auto label_indetifier = extract_token_at(source, pos);
            auto label = shader_out[i + 4];
            labels[label_indetifier] = label;

            for (auto& unresolved_label : unresolved_labels[label_indetifier]) {
                buffer[unresolved_label] = label;
            }

            i += 2;
            continue;
        }
        if (value == GOTO_REPLACE) {
            if (i + 2 >= shader_out_size) throw Exception(ExceptionType::Postprocessor, "Malformed IR. `goto_replace` token at end of shader.");
            buffer.append(IR_REFERNCE);
            auto pos = (u32)shader_out[i + 2];
            auto label = extract_token_at(source, pos);
            if (labels.find(label) != labels.end()) {
                buffer.append(labels[label]);
            } else {
                unresolved_labels[label].push_back(buffer.get_size());
                buffer.append(0);
            }
            i += 2;
            continue;
        }
        if (value == CONTINUABLE) {
            if (i + 2 >= shader_out_size) throw Exception(ExceptionType::Postprocessor, "Malformed IR. `continuable` token at end of shader.");
            
            // TODO
            // This actually needs the next scope. 
            // You also need to do something for single/null satements.
            continue_stack.push({ shader_out[i + 2], scope_stack.peek() });
            continue;
        }
        if (value == SCOPE_START) {
            scope_stack.push(shader_out[i + 2]);
            i += 3;
            continue;
        }
        if (value == SCOPE_END) {
            auto scope = scope_stack.pop();
            
            if (!continue_stack.empty() && continue_stack.peek().scope == scope) {
                continue_stack.pop();
            }

            i += 1;
            continue;
        }
        if (value == CONTINUE) {
            if (continue_stack.empty()) throw Exception(ExceptionType::Postprocessor, "Malformed IR. `continue` token outside of loop.");
            auto continue_ = continue_stack.peek().index;
            buffer.append(JMP);
            buffer.append(IR_REFERNCE);
            buffer.append(continue_);
            continue;
        }
        if (value == BREAK) {
            buffer.append(JMP); 
            buffer.append(IR_REFERNCE);
            break_stack.push(buffer.get_size());
            buffer.append(0);
            continue;
        }
        if (value == BREAKABLE) {
            while (!break_stack.empty()) {
                buffer[break_stack.pop()] = shader_out[i + 2];
            }
            continue;
        }
    
        buffer.append(value);
    }

    return {buffer.flattern(), buffer.get_size()};
}