#include "preprocessor.h"

#include "utils.h"
#include "exception.h"
#include "job.h"

#include <string.h>
#include <algorithm>

// Public interface
std::string preprocess(std::string filename, VariableRegistry& var_reg) {
    ExtendableBuffer<u8> buffer = ExtendableBuffer<u8>(1024);
    auto pre_proc_tokens = std::unordered_map<std::string, PreprocessorMacro*>();
    pre_proc_tokens["__BASE_FILE__"] = new ObjectLikeMacro(filename);
    auto included = std::vector<std::string>();

    try {
        preprocessor::preprocess(filename.c_str(), &buffer, included, var_reg, pre_proc_tokens);
    } catch (Exception& e) {
        if (e.type == ExceptionType::Misc)
            e.type = ExceptionType::Preprocessor;
        throw e;
    }

    for (auto& pair : pre_proc_tokens) {
        delete pair.second;
    }
    
    buffer.add((u8*)" 0EOF\0", 6);

    // FIXME: either free `buffer` after its a string or just don't use an `std::string` so it's not copied.
    return std::string((char*)buffer.flattern());
}

namespace preprocessor {
    // Internal functions

    std::string consume_string_literal(const char* source, u32& i, 
        const char closer, VariableRegistry& var_reg) {

        u32 start = i;
        for (i++; source[i] != '\0'; i++) {
            // TODO: Escape sequences
            if (source[i] == '\\') {
                i++;
                continue;
            }
            if (source[i] == closer) {
                i++;
                return var_reg.append_string_literal((char*)source, i, i - start);
            }
        }
        throw Exception("Unterminated string literal.");
    }

    void consume_line_comment(const char* source, u32& i) {
        for (i += 2; source[i] != '\0'; i++) {
            // TODO: Deal with line endings properly
            if (source[i] == '\n' || source[i] == '\r') {
                i++;
                return;
            }
        }
    }

    void consume_multi_line_comment(const char* source, u32& i) {
        for (i += 2; source[i] != '\0'; i++) {
            if (source[i] == '*' && source[i + 1] == '/') {
                i += 2;
                return;
            }
        }
        throw Exception("Unterminated multi-line comment.");
    }

    void flush_token_buffer(u32& token_buffer_ptr) {
        token_buffer_ptr = 0;
    }

    void handle_token_buffer(u32& token_buffer_ptr, char* token_buffer, u32& i, const char* source,
        ExtendableBuffer<u8>* buffer, PreProcTokensMap& pre_proc_tokens) {
        
        if (token_buffer_ptr > 0) {
            token_buffer[token_buffer_ptr] = '\0';
            auto token = std::string(token_buffer);

            // if (token == "typedef") {
            //     // TODO
            //     u32 start = buffer->get_size() - 1;
            //     while (start > 0 && (*buffer)[start] != ';' && (*buffer)[start] != '}' && (*buffer)[start] != '{') start--;
            //     start++;
            //     auto pre = buffer->rollback(buffer->get_size() - start);
            //     u32 end = i;
            //     while (source[end] != '\0' && source[end] != ';' && source[end] != '}') end++;
                
            //     std::string type = "";
            //     for (auto& c : pre) {
            //         type += (char)c;
            //     }
            //     for(i++; i < end; i++) {
            //         type += source[i];
            //     }
            //     printf("typedef: %s\n", type.c_str());

            //     buffer->add((u8*)"fuck you", 8);

            //     flush_token_buffer(token_buffer_ptr);
            // }

            // FIXME: use the map properly I think it's being queried twice or something.
            auto search = pre_proc_tokens.find(token);
            if (search == pre_proc_tokens.end()) {
                buffer->add((u8*)token_buffer, token_buffer_ptr * sizeof(char));
            } else {
                // FIXME: variadic macros https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html#Variadic-Macros
                

                search->second->exectue(buffer, i, source);
                //buffer->add((u8*)repl.c_str(), repl.length() * sizeof(char));
            }

            flush_token_buffer(token_buffer_ptr);
        }
    }

    void skip_space(const char* source, u32& i) {
        while(source[i] != '\0' && source[i] == ' ') i++;
    }

    std::string next_token(u32& i, const char* source, char* token_buffer, u32& token_buffer_ptr) {
        i++;
        skip_space(source, i); // spaces are allowed here for some reason...
        flush_token_buffer(token_buffer_ptr);
        while(source[i] != '\0' && char_utils::is_alpha_numeric_or_underscore(source[i])) {
            token_buffer[token_buffer_ptr++] = source[i];
            i++;
        }
        token_buffer[token_buffer_ptr] = '\0';
        return std::string(token_buffer);
    }

    enum class IncludeType: u32 {
        System,
        Local,
    };

    void handle_include(u32& i, const char* source, char* token_buffer,
        u32& token_buffer_ptr, ExtendableBuffer<u8>* buffer,
        VariableRegistry& var_reg, PreProcTokensMap& pre_proc_tokens, const char* filename, std::vector<std::string>& included) {

        skip_space(source, i); // FIXME: newlines and stuff
        
        if (source[i] != '"' && source[i] != '<') throw Exception("Expected '\"' or '<' after #include");
        
        auto include_type = source[i++] == '<' ? IncludeType::System : IncludeType::Local;
        auto name = std::string();
        while(source[i] != (include_type == IncludeType::System ? '>' : '"')) {
            name += source[i++];
            if (source[i] == '\0') throw Exception("Unterminated include.");
        }

        if (include_type == IncludeType::System) {
            name = Job::libc_path + name;
        } else {
            // This is also meant to check the include path or something 
            // https://gcc.gnu.org/onlinedocs/cpp/Search-Path.html#Search-Path
            u32 last_slash = strlen(filename);
            while(last_slash > 0 && filename[last_slash] != '/') last_slash--;
            name = std::string(filename, last_slash) + "/" + name;
        }
        for (auto& inc : included) {
            if (inc == name) return;
        }    
        preprocessor::preprocess(name.c_str(), buffer, included, var_reg, pre_proc_tokens);
    }

    void handle_define(u32& i, const char* source, char* token_buffer, 
        u32& token_buffer_ptr, ExtendableBuffer<u8>* buffer,
        VariableRegistry& var_reg, PreProcTokensMap& pre_proc_tokens, const char* filename) {

        auto macro_name = next_token(i, source, token_buffer, token_buffer_ptr);
        enum { ObjectLike, FunctionLike } macro_type = source[i] == '('
            ? FunctionLike 
            : ObjectLike
        ;
        
        auto macro_repl = std::string();
        while(source[i] != '\0') {
            if (source[i] == '\\' && source[i + 1] == '\n' || source[i + 1] == '\r' ) {
                if (source[i + 1] == '\r' && source[i + 2] == '\n') 
                    i += 3;
                else
                    i += 2;
                macro_repl += '\n';
                continue;
            }
            if (source[i] == '\r' || source[i] == '\n') break;
            macro_repl += source[i++];
        }

        pre_proc_tokens[macro_name] = macro_type == ObjectLike
            ? (PreprocessorMacro*) new ObjectLikeMacro(macro_repl)
            : (PreprocessorMacro*) new FunctionLikeMacro(macro_repl)
        ;
    }

    void handle_pre_proc_directive(u32& i, const char* source, char* token_buffer, 
        u32& token_buffer_ptr, ExtendableBuffer<u8>* buffer, i32& if_depth,
        VariableRegistry& var_reg, PreProcTokensMap& pre_proc_tokens, const char* filename, std::vector<std::string>& included) {

        auto name = next_token(i, source, token_buffer, token_buffer_ptr);

        if (if_depth > 0) {
            if ("endif" == name) if_depth--;
            else if ("if" == name || "ifdef" == name || "ifndef" == name) if_depth++;
            return;
        }

        // TODO: micro optimise this with map or whatever
        if ("import" == name) {
            auto empty = std::vector<std::string>();
            handle_include(i, source, token_buffer, token_buffer_ptr, buffer, var_reg, pre_proc_tokens, filename, empty);
        } else
        if ("include" == name) {
            handle_include(i, source, token_buffer, token_buffer_ptr, buffer, var_reg, pre_proc_tokens, filename, included);
        } else
        if ("define" == name) {
            handle_define(i, source, token_buffer, token_buffer_ptr, buffer, var_reg, pre_proc_tokens, filename);
        } else
        if ("undef" == name) {
            auto macro_name = next_token(i, source, token_buffer, token_buffer_ptr);
            pre_proc_tokens.erase(macro_name);
        } else
        if ("ifdef" == name) {
            auto macro_name = next_token(i, source, token_buffer, token_buffer_ptr);
            if (pre_proc_tokens.find(macro_name) == pre_proc_tokens.end())
                if_depth++;
        } else
        if ("ifndef" == name) {
            auto macro_name = next_token(i, source, token_buffer, token_buffer_ptr);
            if (pre_proc_tokens.find(macro_name) != pre_proc_tokens.end())
                if_depth++;
        } else
        if ("endif" == name) {
            return;
        } else
        if ("if" == name) {
            // TODO
            throw Exception("#if is not supported yet.");
        } else
        if ("elif" == name) {
            // TODO
            throw Exception("#elif is not supported yet.");
        } else
        if ("else" == name) {
            // TODO
            throw Exception("#else is not supported yet.");
        } else 
        if ("pragma" == name) {
            // TODO
            throw Exception("#pragma is not supported yet.");
        } else {
            throw Exception("Unknown preprocessor directive \"#" + name + "\".");
        }
    }

    void preprocess(const char* filename, ExtendableBuffer<u8>* buffer, std::vector<std::string>& included,
        VariableRegistry& var_reg, PreProcTokensMap& pre_proc_tokens) {

        included.push_back(filename);
        pre_proc_tokens["__FILE__"] = new ObjectLikeMacro(filename);

        auto source = load_file(filename);
        
        #define TOKEN_BUFFER_SIZE 1024
        char token_buffer[TOKEN_BUFFER_SIZE];
        u32 token_buffer_ptr = 0;
        i32 if_depth = 0;
        u32 i;
        for (i = 0; source[i] != '\0'; i++) {
            // Cry about it.
            goto no_flush;
            flush_continue:
                flush_token_buffer(token_buffer_ptr);
                if (source[++i] == '\0') break;
            no_flush:

            char c = source[i];
            if (c == '"') {
                std::string string_literal_identifier = consume_string_literal(source, i, '"', var_reg);
                if (if_depth == 0)
                    buffer->add((u8*)string_literal_identifier.c_str(), 
                        string_literal_identifier.length() * sizeof(char));
                goto flush_continue;
            }
            if (source[i] == '/' && source[i + 1] == '/') {
                consume_line_comment(source, i);
                goto flush_continue;
            }
            if (source[i] == '/' && source[i + 1] == '*') {
                consume_multi_line_comment(source, i);
                goto flush_continue;
            }
            if (c == '#') {
                // FIXME: Error if not start of line
                handle_pre_proc_directive(i, source, token_buffer, token_buffer_ptr, 
                    buffer, if_depth, var_reg, pre_proc_tokens, filename, included);
                goto flush_continue;
            }

            if (char_utils::is_alpha_numeric_or_underscore(c)) {
                token_buffer[token_buffer_ptr++] = c;
                if (token_buffer_ptr >= TOKEN_BUFFER_SIZE) throw Exception("Token buffer overflow.");
                continue;
            } else {
                if (if_depth == 0) {
                    handle_token_buffer(token_buffer_ptr, token_buffer, i, source, buffer, pre_proc_tokens);
                    buffer->append(c);
                }
            }
        }
        // Incase there was a token at the end of the file.
        handle_token_buffer(token_buffer_ptr, token_buffer, i, source, buffer, pre_proc_tokens);
    }
}