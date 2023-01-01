#include "compiler.h"
#include "utils.h"
#include "gl.h"
#include "shader.h"
#include "lang.h"
#include "yacc.h"
#include "ir_compiler.h"

#include "c_tokens.cpp"
#include "c_grammar.cpp"


#include "include/glad/glad.h"
#include <math.h>

struct Token {
	GLuint id;
	GLuint len;
    GLint ast_node_location;
};

Ssbo* tokenise(UintString& source, Shader* tokeniser) {
    Ssbo source_ssbo = Ssbo(source.length * sizeof(GLuint), source.data);
    source_ssbo.bind(1);
    Ssbo* tokens = new Ssbo(source.length * sizeof(Token));
    tokens->bind(2);
    tokeniser->exec(ceil((source.length / 32) / 4)+1);
    return tokens;
}

std::string load_source(std::vector<std::string> source_files) {
    // TODO: separate files appropriately
    std::string source = std::string();
    for (std::string _source : source_files) {
        source += load_file(_source.c_str());
    }
    return source;
}

struct ChildNode {
    GLint ref;
    GLuint codegenVolume;
};

struct AstNode {
	GLuint nodeToken;
	ChildNode children[4];
    GLuint volume;
};

#define MAX_AST_NODES 30

void compile(Job& job)
{
    if (job.source_files.size() == 0) { throw "No source files specified."; }

    std::string source_str = load_source(job.source_files);
    auto source = to_uint_string(source_str);


    Shader tokeniser = Shader("shaders/tokeniser.glsl", GL_ALL_BARRIER_BITS);
    Shader ast = Shader("shaders/ast.glsl", GL_ALL_BARRIER_BITS);
    Shader codegen = Shader("shaders/codegen.glsl", GL_ALL_BARRIER_BITS);

    auto lang_tokens_parse_tree = tokens_list_to_parse_tree(c_tokens);

    auto pt_ssbo = lang_tokens_parse_tree->into_ssbo();
    pt_ssbo->bind(0);




    auto tokens = tokenise(source, &tokeniser);
    tokens->print_contents();

    IrTokenList* ir_tokens = new IrTokenList();
    auto ast_ssbos = create_ast_ssbos(c_yacc, *lang_tokens_parse_tree, ir_tokens);


    ast_ssbos.ast_parse_tree->bind(4);
    ast_ssbos.ast_nodes->bind(1);

    auto ast_nodes = Ssbo(MAX_AST_NODES * sizeof(AstNode));
    ast_nodes.bind(3);
    ast_ssbos.ir_codegen->bind(0);

    for (int i = 0; i < 200; i++)
        ast.exec((tokens->size / 32) / 4 + 1);

    tokens->print_contents();
    printf("AST NODES:\n");
    ast_nodes.print_contents();
    
    #define OUTPUT_BUFFER_SIZE 100

    Ssbo output_buffer = Ssbo(OUTPUT_BUFFER_SIZE * sizeof(GLuint));
    output_buffer.bind(1);


    codegen.exec(1);
    //output_buffer.print_contents();


    GLuint* out_buf_dmp = (GLuint*)output_buffer.dump();
    auto s = serialize_uir_to_readable(out_buf_dmp, OUTPUT_BUFFER_SIZE, *ir_tokens, source_str);
    printf("OUTPUT:\n%s\n", s.c_str());

    delete_ast_ssbos(ast_ssbos);
    free(out_buf_dmp);
    delete ir_tokens;
    delete lang_tokens_parse_tree;
    delete[] source.data;
    delete tokens;
}