#include "compiler.h"
#include "utils.h"
#include "gl.h"
#include "shader.h"
#include "lang.h"
#include "yacc.h"
#include "ir_compiler.h"

#include "c_tokens.cpp"
#include "c_grammar.cpp"

#include "debug_printing.h"
#include "shader_structures.h"

#include "include/glad/glad.h"
#include <math.h>


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

#define MAX_AST_NODES 50

std::string compile(Job& job, Shaders& shaders) {
    if (job.source_files.size() == 0) { throw "No source files specified."; }
    
    std::string source_str = load_source(job.source_files);
    auto source = to_uint_string(source_str);


    auto lang_tokens_parse_tree = tokens_list_to_parse_tree(c_tokens);

    auto pt_ssbo = lang_tokens_parse_tree->into_ssbo();
    pt_ssbo->bind(0);




    auto tokens = tokenise(source, &shaders.tokeniser);

    if (job.dbg) tokens->print_contents();
    
    // IDK what the fuck is going on here 
    tokens->dump();

    IrTokenList* ir_tokens = new IrTokenList();
    ParseTree yacc_parse_tree = ParseTree({}, true);
    auto ast_ssbos = create_ast_ssbos(c_yacc, *lang_tokens_parse_tree, ir_tokens, yacc_parse_tree);


    ast_ssbos.ast_parse_tree->bind(4);
    ast_ssbos.ast_nodes->bind(1);

    auto ast_nodes = Ssbo(MAX_AST_NODES * sizeof(AstNode));
    ast_nodes.bind(3);
    ast_ssbos.ir_codegen->bind(0);

    if (job.dbg) {
        auto tokens_dmp = tokens->dump();
        print_tokens(tokens_dmp, tokens->size, *lang_tokens_parse_tree, *lang_tokens_parse_tree);
        free(tokens_dmp);
    }

    for (int i = 0; i < 200; i++)
        shaders.ast.exec((tokens->size / 32) / 4 + 1);

    if (job.dbg) {
        tokens->print_contents();

        auto tokens_dmp = tokens->dump();
        print_tokens(tokens_dmp, tokens->size, *lang_tokens_parse_tree, yacc_parse_tree);
        free(tokens_dmp);

        printf("AST NODES:\n");
        ast_nodes.print_contents();

        // auto nodes = ast_nodes.dump();
        // print_ast_nodes(nodes, ast_nodes.size);
        // free(nodes);
    }
    
    #define OUTPUT_BUFFER_SIZE 200

    Ssbo output_buffer = Ssbo(OUTPUT_BUFFER_SIZE * sizeof(GLuint));
    output_buffer.bind(1);


    shaders.codegen.exec(2);


    GLuint* out_buf_dmp = (GLuint*)output_buffer.dump();
    auto s = serialize_uir_to_readable(out_buf_dmp, OUTPUT_BUFFER_SIZE, *ir_tokens, source_str);
    
    if (job.dbg) printf("OUTPUT:\n%s\n", s.c_str());

    delete_ast_ssbos(ast_ssbos);
    free(out_buf_dmp);
    delete ir_tokens;
    delete lang_tokens_parse_tree;
    delete[] source.data;
    delete tokens;

    return s;
}