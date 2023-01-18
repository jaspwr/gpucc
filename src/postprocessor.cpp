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

void replace_identifier(const GLint* shader_out, u32 ref_pos,
    VariableRegistry& var_reg, std::string& source) {
    
    if (shader_out[ref_pos] != IR_SOURCE_POS_REF) return;
    auto pos = (u32)shader_out[ref_pos + 1];
    auto token = extract_token_at(source, pos);
    if (char_utils::is_numeric(token[0])) return;
    auto var = var_reg.get_var(token);

    GLint* w_shader_out = (GLint*)shader_out;
    w_shader_out[ref_pos] = IR_REFERNCE;
    w_shader_out[ref_pos + 1] = var->register_;
}

void insert_at(ExtendableBuffer<GLint>& buffer, u32 rollback_len, std::vector<GLuint>& insertion, GLuint NEWLINE) {
    auto line = buffer.rollback(rollback_len);
    for (auto token : insertion) {
        buffer.append(token);
    }
    buffer.append(NEWLINE);
    for (auto token : line) {
        buffer.append(token);
    }
}

void insert_line (ExtendableBuffer<GLint>& buffer, GLuint NEWLINE, std::vector<GLuint> insertion) {
    u32 existing_line_len =  0;
    while (buffer[buffer.get_size() - existing_line_len - 1] != NEWLINE) existing_line_len++;
    insert_at(buffer, existing_line_len, insertion, NEWLINE);
}

struct Switch {
    u32 insert_position;
    
    Switch(u32 insert_position) {
        this->insert_position = insert_position;
    }
};

void handle_break(Stack<GLint>& break_stack, ExtendableBuffer<GLint>& buffer, GLuint JMP) {
    buffer.append(JMP); 
    buffer.append(IR_REFERNCE);
    break_stack.push(buffer.get_size());
    buffer.append(0);
}

void handle_breakable_label(const GLint* shader_out, Stack<GLint>& break_stack, ExtendableBuffer<GLint>& buffer, u32& i) {
    auto label = shader_out[i + 2];
    while (!break_stack.empty()) {
        buffer[break_stack.pop()] = label;
    }
}

struct continue_ {
    GLint index;
    GLint scope;
};

void handle_continue(Stack<continue_>& continue_stack, ExtendableBuffer<GLint>& buffer, GLuint JMP) {
    if (continue_stack.empty()) throw Exception(ExceptionType::Postprocessor, "Malformed IR. `continue` token outside of loop.");
    auto continue_ = continue_stack.peek().index;
    buffer.append(JMP);
    buffer.append(IR_REFERNCE);
    buffer.append(continue_);
}

typedef std::unordered_map<std::string, GLint> LabelMap;
typedef std::unordered_map<std::string, std::vector<GLint>> UnresolvedLabelMap;

void handle_gotoable_label(const GLint* shader_out, GLuint shader_out_size, u32& i,
    ExtendableBuffer<GLint>& buffer, std::string& source, LabelMap& labels, 
    UnresolvedLabelMap& unresolved_labels) {

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
}

void handle_goto_replace(const GLint* shader_out, GLuint shader_out_size, u32& i,
    ExtendableBuffer<GLint>& buffer, std::string& source, LabelMap& labels, 
    UnresolvedLabelMap& unresolved_labels) {

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
}

void handle_identifier(const GLint* shader_out, std::string& source, ExtendableBuffer<GLint>& buffer,
    VariableRegistry& var_reg, u32& i, GLuint NEWLINE, GLuint EQUAL, GLuint LOAD, bool loadable_flag) {

    replace_identifier(shader_out, i, var_reg, source);

    auto marker = shader_out[i];
    auto reg = shader_out[++i];
    buffer.append(marker);
    if (loadable_flag && marker == IR_REFERNCE && var_reg.is_loadable(reg)) {
        auto new_reg = var_reg.get_new_register();
        insert_line(buffer, NEWLINE, std::vector<GLuint>{
            IR_REFERNCE, new_reg, EQUAL, LOAD, IR_REFERNCE, (GLuint)reg
        });
        buffer.append(new_reg);
    } else {
        buffer.append(reg);
    }
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
    auto EQUAL = tokenise_ir_token("=", ir_tokens);
    auto LOAD = tokenise_ir_token("LOAD", ir_tokens);
    auto GOTOABLE = tokenise_ir_token("gotoable", ir_tokens);
    auto GOTO_REPLACE = tokenise_ir_token("goto_replace", ir_tokens);
    auto REPLACE_ME = tokenise_ir_token("replace_me", ir_tokens);
    auto REPLACE_WITH = tokenise_ir_token("replace_with", ir_tokens);
    auto ALLOCA = tokenise_ir_token("ALLOCA", ir_tokens);
    auto NEWLINE = tokenise_ir_token("\n", ir_tokens);
    auto SWITCH = tokenise_ir_token("switch", ir_tokens);
    auto SWITCH_BODY_END = tokenise_ir_token("switch_body_end", ir_tokens);
    auto SWITCH_CASE = tokenise_ir_token("switch_case", ir_tokens);
    auto SWITCH_DEFAULT = tokenise_ir_token("switch_default", ir_tokens);
    auto FN = tokenise_ir_token("fn", ir_tokens);

    Stack<GLint> scope_stack("scope_stack");
    scope_stack.push(0); // global scope
    Stack<continue_> continue_stack("continue_stack");
    Stack<GLint> continues_for_next_scope("continues_for_next_scope");

    Stack<GLint> break_stack("break_stack");
    Stack<GLint> replace_stack("replace_stack");
    Stack<Switch> switch_stack("switch_stack");
    LabelMap labels;
    UnresolvedLabelMap unresolved_labels;

    bool loadable_flag = false;

    for (u32 i = 0; i < shader_out_size; i++) {
        auto value = shader_out[i];
        if ((value == IR_SOURCE_POS_REF || value == IR_REFERNCE)) {
            if (var_reg.is_in_global_scope()) {
                buffer.append(shader_out[i]);
                buffer.append(shader_out[++i]);
                continue;
            }

            handle_identifier(shader_out, source, buffer, var_reg, i, NEWLINE, EQUAL, LOAD, loadable_flag);
            
            loadable_flag = false;
            continue;
        }
        if (value == REPLACE_WITH) {
            replace_stack.push(shader_out[i + 2]);
            i += 3;
            continue;
        }
        if (value == SWITCH) {
            switch_stack.push(Switch(buffer.get_size() + 5));
        }
        if (value == SWITCH_BODY_END) {
            switch_stack.pop();
            continue;
        }
        if (value == SWITCH_CASE) {
            auto& switch_ = switch_stack.peek();
            auto case_value = (GLuint)shader_out[i + 2];
            auto case_label = (GLuint)shader_out[i + 4];
            auto insertion = std::vector<GLuint>{IR_SOURCE_POS_REF, case_value, IR_REFERNCE, case_label};
            
            // FIXME: this dosent work
            // insert_at(buffer, buffer.get_size() - switch_.insert_position, insertion, NEWLINE);
            
            i += 2;
            continue;
        }
        if (value == SWITCH_DEFAULT) {
            auto& switch_ = switch_stack.peek();
            auto case_label = (GLuint)shader_out[i + 2];
            auto insertion = std::vector<GLuint>{JMP, IR_REFERNCE, case_label};

            // insert_at(buffer, buffer.get_size() - switch_.insert_position, insertion, NEWLINE);
            continue;
        }

        if (value == REPLACE_ME) {
            if (replace_stack.size() == 0) continue; //throw Exception(ExceptionType::Postprocessor, "Malformed IR. `replace_me` token with no `replace_with` token.");
            buffer.append(IR_REFERNCE);
            buffer.append(replace_stack.pop());
            continue;
        }
        if (value == LOADABLE) {
            loadable_flag = true;
            continue;
        }
        if (value == ALLOCA) {
            // TODO: deal w types
            auto pos = (u32)shader_out[i + 3];
            auto name = extract_token_at(source, pos);
            auto val = new TypedValue();
            val->register_ = shader_out[i - 2];
            val->data = nullptr;
            var_reg.add_var(name, val);
        }
        if (value == GOTOABLE) {
            handle_gotoable_label(shader_out, shader_out_size, i, buffer, source, labels, unresolved_labels);
            continue;
        }
        if (value == GOTO_REPLACE) {
            handle_goto_replace(shader_out, shader_out_size, i, buffer, source, labels, unresolved_labels);
            continue;
        }
        if (value == CONTINUABLE) {
            if (i + 2 >= shader_out_size) throw Exception(ExceptionType::Postprocessor, "Malformed IR. `continuable` token at end of shader.");
            
            // TODO
            // do something for single/null satements.
            continues_for_next_scope.push(shader_out[i + 2]);
            continue;
        }
        if (value == SCOPE_START) {
            auto scope_ref = shader_out[i + 2];
            scope_stack.push(scope_ref);
            
            while (!continues_for_next_scope.empty()) {
                continue_stack.push({continues_for_next_scope.pop(), scope_ref});
            }

            var_reg.push_scope();
            i += 3;
            continue;
        }
        if (value == SCOPE_END) {
            auto scope = scope_stack.pop();
            var_reg.pop_scope();
            
            if (!continue_stack.empty() && continue_stack.peek().scope == scope) {
                continue_stack.pop();
            }

            i += 1;
            continue;
        }
        if (value == CONTINUE) {
            handle_continue(continue_stack, buffer, JMP);
            continue;
        }
        if (value == BREAK) {
            handle_break(break_stack, buffer, JMP);
            continue;
        }
        if (value == BREAKABLE) {
            handle_breakable_label(shader_out, break_stack, buffer, i);
            continue;
        }
    
        buffer.append(value);
    }

    return {buffer.flattern(), buffer.get_size()};
}