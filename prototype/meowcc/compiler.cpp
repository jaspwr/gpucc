#include "compiler.h"
// TODO: Make this more flexible and not just hard coded for c

enum preproc_context {
    code,
    preprocessor_dir,
    comment,
    string_literal
};
struct state_change {
    const char* open;
    const char* close;
    preproc_context context;
};

const auto default_context = preproc_context::code;
state_change state_changes[] = {
    {"#","\n",preproc_context::preprocessor_dir},
    {"//","\n",preproc_context::comment},
    {"/*","*/",preproc_context::comment},
    {"\"","\"",preproc_context::string_literal}
};


preprocessor_directive IMPORT("import");
preprocessor_directive INCLUDE("include");
preprocessor_directive DEFINE("define");
preprocessor_directive UNDEF("undef");
preprocessor_directive IF("if");
preprocessor_directive IFDEF("ifdef");
preprocessor_directive IFNDEF("ifndef");
preprocessor_directive PRAGMA("pragma");
preprocessor_directive ERROR("error");


#define BUFFER_SIZE 20
char preprocessor_directive_buffer[20];
int preprocessor_directive_buffer_ptr = 0;
int parse_preprocessor_directive(char c) {
    if (c < '!') {
        if (preprocessor_directive_buffer_ptr == 0)
            return -1;
        preprocessor_directive_buffer[preprocessor_directive_buffer_ptr] = '\0';
        preprocessor_directive_buffer_ptr = 0;
        for (preprocessor_directive* ppd : preprocessor_directive::self_list) {
            if (utils::str_match((char*)(ppd->string), (char*)preprocessor_directive_buffer)) {
                return ppd->val;
            }
        }
        throw "Invalid preprocessor directive";
    } 
    else {
        preprocessor_directive_buffer[preprocessor_directive_buffer_ptr] = c;
        preprocessor_directive_buffer_ptr++;
        return -1;
    }
}
#undef BUFFER_SIZE

enum import_types {
    nothing,
    quote_marks,
    angle_brackets
};
import_types import_type = nothing;
std::string import_file_path_buffer;
bool parse_import_file_path(char c) {
    switch (import_type)
    {
    case nothing:
        import_file_path_buffer = "";
        if (c == '\"')
            import_type = quote_marks;
        else if (c == '<')
            import_type = angle_brackets;
        break;
    case quote_marks:
        if (c == '\"')
            return true;
        else
            import_file_path_buffer += c;
        break;
    case angle_brackets:
        if (c == '>')
            return true;
        else
            import_file_path_buffer += c;
        break;
    default:
        break;
    }
    return false;
}

std::string get_source_file_location(const char* source_file_path) {
    //TODO: add support for both '/' and '\'

    auto source_file_location = (std::string)source_file_path;
    return source_file_location.substr(0, source_file_location.find_last_of('/') + 1);
}

void copy_to_buffer(char * source, char* destination, int source_len, int dest_pos) {
    for (int i = 0; i < source_len; i++) {
        destination[i + dest_pos] = source[i];
    }
}



const int recursion_limit = 500;
int calls = 0;
struct load_source_ret { char* data; int len; int location = 0; };
bool load_source_ret_compare(load_source_ret& r1, load_source_ret& r2) {
    return r1.location > r2.location;
}
load_source_ret load_and_preprocess_source_file(const char* source_file_path) {
    calls++;
    if (calls > recursion_limit)
        throw;
    std::ifstream is(source_file_path, std::ifstream::binary);
    if (!is.is_open())
    {
        printf("[E] Could not read \"%s\"", source_file_path);
        exit(1);
    }
    is.seekg(0, is.end);
    int length = is.tellg();
    is.seekg(0, is.beg);
    char* buffer = new char[length];
    is.read(buffer, length);
    is.close();

    int char_ptr = 0;
    preproc_context context{};


    auto source_file_location = get_source_file_location(source_file_path);
    auto children = new std::list<load_source_ret>{};
    int child_count = 0;
    int preprocessor_directive_token = -1;
    
    for (int i = 0; i < length; i++) {
        for (state_change& stchng : state_changes)
        {
            if (stchng.open[0] == buffer[i]) {
                int open_char_ptr = 1;
                bool match = true;
                while (stchng.open[open_char_ptr] != '\0') {
                    i++;
                    if (stchng.open[open_char_ptr] != buffer[i]) {
                        match = false;
                        i -= open_char_ptr;
                        break;
                    }
                    open_char_ptr++;
                }
                if (match) {
                    context = stchng.context;
                    while (true) {
                        if (buffer[i] == stchng.close[0]) {
                            int close_char_ptr = 1;
                            bool match = true;
                            while (stchng.close[close_char_ptr] != '\0') {
                                i++;
                                if (stchng.close[close_char_ptr] != buffer[i]) {
                                    match = false;
                                    i -= close_char_ptr;
                                    break;
                                }
                                close_char_ptr++;
                            }
                            if (match)
                            {
                                preprocessor_directive_token = -1;
                                context = default_context;
                                i++;
                                break;
                            }
                        }
                        if (context == preproc_context::preprocessor_dir) {
                            if (preprocessor_directive_token == -1) {
                                preprocessor_directive_token = parse_preprocessor_directive(buffer[i + 1]);
                            }
                            else {
                                if (preprocessor_directive_token == IMPORT.val)
                                {
                                    if (parse_import_file_path(buffer[i + 1])) {
                                        children->push_back(load_and_preprocess_source_file(
                                            (source_file_location + import_file_path_buffer).c_str()));
                                        child_count++;
                                        children->back().location = char_ptr;
                                        import_file_path_buffer = "";
                                    }
                                }
                            }
                        }
                        else if (context == preproc_context::string_literal) {
                            buffer[char_ptr] = buffer[i];
                            char_ptr++;
                        }
                        i++;
                        if (i >= length)
                            goto escape_nested_loops;
                    }
                    break;
                }
            }
        }
        buffer[char_ptr] = buffer[i];
        char_ptr++;
    }
escape_nested_loops:
    buffer[char_ptr-1] = '\0';
    if(child_count > 0){
        int total_size = char_ptr;
        for (auto& child : *children) {
            total_size += child.len;
        }
        auto return_buffer = new char[total_size];
        children->sort(load_source_ret_compare);
        load_source_ret curr = children->front();
        children->pop_front();
        child_count--;
        bool copying_child = false;
        int in_child_index = 0;
        int in_self_index = 0;
        char* pre_curr = nullptr;
            
        for (int i = 0; i < total_size; i++) {
            if (copying_child) {
                return_buffer[i] = curr.data[in_child_index];
                in_child_index++;
                if (in_child_index > curr.len-1)
                    copying_child = false;
            }
            else {
                return_buffer[i] = buffer[in_self_index];
                in_self_index++;
                if (in_self_index == curr.location) {
                    in_child_index = 0;
                    copying_child = true;
                    if(pre_curr != nullptr)
                        delete[] pre_curr;
                    pre_curr = curr.data;
                    if(child_count > 0){
                        curr = children->front();
                        children->pop_front();
                    }
                    
                }
            }
        }
        delete children;
        delete[] buffer;
        return_buffer[total_size-1] = '\0';
    
        return { return_buffer, total_size };
    }
    else {
        return { buffer, char_ptr };
    }
}

const char* source;
preproc_ret compiler::preprocessor(const char* source_file_path) {
    auto r = load_and_preprocess_source_file(source_file_path);
    source = r.data;
    GLuint c = load_to_vram((unsigned char*)source, r.len, 1, GL_RGBA32F, GL_RGBA);
    return { c, r.len };
}


int compiler::token_length(const char* source, int start_position) {
	// TODO: Add some more saftey nets here there is a lot of unsafe pointer arithmetic.
    //       Buffer overflow checking is definantly needed.
	// TODO: This is hardcoded for C and should be made universal.
    source += start_position;
    int init_ptr = (int)source;
    int _class;

#define CLASSIFY_TOKEN if (*source < '!') \
        _class = 0; \
    else if (*source >= '0' && *source <= '9') \
        _class = 1; \
    else if (*source >= 'A' && *source <= 'Z') \
        _class = 1; \
    else if (*source >= 'a' && *source <= 'z') \
        _class = 1; \
    else \
        _class = 2;
	
    CLASSIFY_TOKEN
    int pre_class = _class;
	
    int safety_counter = 0;
	while (_class == pre_class && safety_counter < 1000) {
		pre_class = _class;
        source++;
        if (*source == '\0')
            break;
        CLASSIFY_TOKEN
        safety_counter++;
    }
    return (int)source - init_ptr;
	
#undef CLASSIFY_TOKEN;
}

int * compiler::compile(GLuint source_vram, int source_len)
{
    const int bind_size = 8;
    shader_binding shader_bindings[bind_size];
    // shader_bindings[0] = {0,c,GL_READ_ONLY,GL_RG32F};
    // shader_bindings[1] = {1,screenTex,GL_WRITE_ONLY,GL_RG32F};
    // shader_bindings[2] = {2,tree,GL_READ_ONLY,GL_RG32F};
    // shader_bindings[3] = {3,cst,GL_READ_ONLY,GL_RG32F};
    shader_bindings[0].texture = source_vram;
    shader_bindings[0].access = GL_READ_ONLY;
    shader_bindings[0].format = GL_RGBA32F;
    shader_bindings[0].type = shader_binding_type::image;
    shader_bindings[2].texture = lang::vram_token_tree;
    shader_bindings[2].access = GL_READ_ONLY;
    shader_bindings[2].format = GL_RGBA32F;
    shader_bindings[2].type = shader_binding_type::image;
    shader_bindings[3].texture = lang::vram_cst;
    shader_bindings[3].access = GL_READ_ONLY;
    shader_bindings[3].format = GL_RGBA32F;
    shader_bindings[3].type = shader_binding_type::image;
    //shader_bindings[5].texture = screenTex;
    //shader_bindings[5].access = GL_WRITE_ONLY;
    //shader_bindings[5].format = GL_RGBA32F;
    //shader_bindings[5].type = shader_binding_type::image;

    shader_bindings[1].type = shader_binding_type::none;
    shader_bindings[4].type = shader_binding_type::none;


    GLuint pong_screen = create_ssbo(SCREEN_WIDTH * SCREEN_WIDTH * 3 * sizeof(int));
    shader_bindings[1].texture = pong_screen;
    shader_bindings[1].type = shader_binding_type::ssbo;


    shader shad(shader_bindings, 6, "shaders/tokenizer.glsl");
    shad.exec(ceil(source_len * 8 * 4), 1, 1, true);

    // GLuint pong_screen;
    // glCreateTextures(GL_TEXTURE_2D, 1, &pong_screen);
    // glTextureParameteri(pong_screen, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTextureParameteri(pong_screen, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTextureParameteri(pong_screen, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTextureParameteri(pong_screen, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTextureStorage2D(pong_screen, 1, GL_RGBA32F, SCREEN_WIDTH, SCREEN_HEIGHT);


    int* _data = new int[12000];
    for (int i = 0; i < 12000; i++)
        _data[i] = 0;
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 12000 * sizeof(int), _data, GL_DYNAMIC_COPY); //sizeof(data) only works for statically sized C/C++ arrays.
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
    glFinish();
    delete[] _data;
    const int atomics_count = 3;
    GLuint hi[atomics_count];
    for (int i = 0; i < atomics_count; i++)
        hi[i] = 0;
    GLuint atomics;
    glGenBuffers(1, &atomics);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomics);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * atomics_count, hi, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 3, atomics);

    GLuint __screenTex = create_ssbo(SCREEN_WIDTH * SCREEN_WIDTH * 3 * sizeof(int));


    shader_bindings[0].texture = __screenTex;
    shader_bindings[0].type = shader_binding_type::ssbo;


    shader_bindings[4].texture = ssbo;
    shader_bindings[4].type = shader_binding_type::ssbo;

    GLuint pong_ast_data = create_ssbo(6000 * sizeof(int));
    shader_bindings[6].texture = pong_ast_data;
    shader_bindings[6].type = shader_binding_type::ssbo;

    GLuint extra_parse_tree_data;
    glGenBuffers(1, &extra_parse_tree_data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, extra_parse_tree_data);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 255 * sizeof(parse_tree_extra), lang::_parse_tree_extra, GL_STATIC_READ);
    glFinish();
    shader_bindings[7].texture = extra_parse_tree_data;
    shader_bindings[7].type = shader_binding_type::ssbo;
    shader_bindings[2].texture = lang::vram_codegen;
    shader_bindings[2].type = shader_binding_type::ssbo;

    shader ast_builder(shader_bindings, 8, "shaders/ast_builder.glsl");
    bool ping_pong = true;
    for (int i = 0; i < 200; i++) {
        if (ping_pong) {
            shader_bindings[1].texture = __screenTex;
            shader_bindings[0].texture = pong_screen;
            shader_bindings[4].texture = pong_ast_data;
            shader_bindings[6].texture = ssbo;

        }
        else {
            shader_bindings[1].texture = pong_screen;
            shader_bindings[0].texture = __screenTex;
            shader_bindings[6].texture = pong_ast_data;
            shader_bindings[4].texture = ssbo;
        }
        ping_pong = !ping_pong;
        ast_builder.exec(2048, 1, 1, true);
    }
    if (ping_pong) {
        shader_bindings[1].texture = __screenTex;
        shader_bindings[0].texture = pong_screen;
        shader_bindings[4].texture = pong_ast_data;
        shader_bindings[6].texture = ssbo;

    }
    else {
        shader_bindings[1].texture = pong_screen;
        shader_bindings[0].texture = __screenTex;
        shader_bindings[6].texture = pong_ast_data;
        shader_bindings[4].texture = ssbo;
    }
    shader_bindings[0].texture = __screenTex;
    GLuint organised = create_ssbo(12000 * sizeof(int));
    shader_bindings[3].texture = organised;
    shader_bindings[3].type = shader_binding_type::ssbo;

    if (ping_pong) {
        shader_bindings[1].texture = __screenTex;
        shader_bindings[0].texture = pong_screen;
        shader_bindings[4].texture = pong_ast_data;
        shader_bindings[6].texture = ssbo;

    }
    else {
        shader_bindings[1].texture = pong_screen;
        shader_bindings[0].texture = __screenTex;
        shader_bindings[6].texture = pong_ast_data;
        shader_bindings[4].texture = ssbo;
    }

    GLuint final_output = create_ssbo(8000 * sizeof(int));
    shader_bindings[5].texture = final_output;
    shader_bindings[5].type = shader_binding_type::ssbo;

    shader arranger(shader_bindings, 6, "shaders/arranger.glsl");
    arranger.exec(1280, 1, 1, true);


    shader codegen_shdr(shader_bindings, 6, "shaders/codegen.glsl");
    codegen_shdr.exec(1280, 1, 1, true);

    int* outp = new int[8000]; // freed in main
    GLuint return_output_ssbo = 0;
    switch (debugging_config) {
    case none:
        return_output_ssbo = final_output;
        break;
    case ir_printout:
        return_output_ssbo = final_output;
        break;
    case ast_nodes_printout:
		return_output_ssbo = shader_bindings[4].texture;
        break;
    case tokens_printout:
        return_output_ssbo = shader_bindings[0].texture;
        break;
    }
	
    glGetNamedBufferSubData(return_output_ssbo, 0, 8000 * sizeof(int), outp);
    return outp;
}
