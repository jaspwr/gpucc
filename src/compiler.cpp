#include "compiler.h"
#include "utils.h"
#include "gl.h"
#include "shader.h"
#include "lang.h"
#include "yacc.h"

#include "c_tokens.cpp"

#include "include/glad/glad.h"

struct Token {
	GLuint id;
	GLuint len;
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
    std::string source = "";
    for (std::string _source : source_files) {
        source += load_file(_source.c_str());
    }
    return source;
}

struct AstNode {
	GLuint nodeToken;
	GLuint children[4];
};

#define MAX_AST_NODES 30

void compile(Job& job)
{
    if (job.source_files.size() == 0) { throw "No source files specified."; }
    auto source = to_uint_string(load_source(job.source_files));

    auto lang_tokens_parse_tree = tokens_list_to_parse_tree(tokens);
    auto pt_ssbo = lang_tokens_parse_tree->into_ssbo();
    pt_ssbo->bind(0);

    auto tokeniser = new Shader("shaders/tokeniser.glsl", GL_ALL_BARRIER_BITS);

    auto tokens = tokenise(source, tokeniser);
    tokens->print_contents();


    auto ast_ssbos = create_ast_ssbos("hello : ';' '}' ; mew : hello hello;", *lang_tokens_parse_tree);

    auto ast = new Shader("shaders/ast.glsl", GL_ALL_BARRIER_BITS);

    ast_ssbos.ast_parse_tree->bind(0);
    ast_ssbos.ast_nodes->bind(1);

    auto ast_nodes = new Ssbo(MAX_AST_NODES * sizeof(AstNode));
    ast_nodes->bind(3);

    for (int i = 0; i < 10; i++)
        ast->exec((tokens->size / 32) / 4 + 1);

    tokens->print_contents();
    printf("AST NODES:\n");
    ast_nodes->print_contents();
    
    delete_ast_ssbos(ast_ssbos);

    delete lang_tokens_parse_tree;
    delete ast_nodes;
    delete[] source.data;
    delete tokens;
    delete tokeniser;
}