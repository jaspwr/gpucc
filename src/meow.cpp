// Copyright (C) 2023 Jasper Parker <j@sperp.dev>
//
// This file is part of Meow.
//
// Meow is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// Meow is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Meow. If not, see <https://www.gnu.org/licenses/>.


#include <stdio.h>

#include "cli.h"
#include "compiler.h"
#include "gl.h"
#include "parse_tree.h"
#include "exception.h"
#include "ir_compiler.h"
#include "ir_ssbo_format.h"
#include "yacc.h"
#include "lang.h"
#include "instruction_selection_syntax.h"

#include "c_tokens.cpp"
#include "grammars/c_grammar.cpp"
#include "arch/x86_64/instruction_selection.h"
#include "arch/x86_64/registers.h"

int main(int argc, char** argv) {
    try {
        Gl::init(true);
        Job job = parse_args(argc, argv);

        auto lang_tokens_parse_tree = tokens_list_to_parse_tree(c_tokens);

        IrTokenList* ir_tokens = new IrTokenList();
        ParseTree yacc_parse_tree = ParseTree(1000);
        ParseTree ir_parse_tree = ParseTree(400);
        std::vector<std::string> grammars = { c_pre_yacc, c_yacc };

        auto ast_ssbos = create_ast_ssbos(grammars, *lang_tokens_parse_tree, ir_tokens, yacc_parse_tree, ir_parse_tree);

        InstSelRet inst_sel = parse_instruction_selection(inst_selection_schema, ir_parse_tree);

        write_file((get_bin_dir() + "/shaders/ir_tokens_generated").c_str(),
            ir_tokens_shader_definitions(*ir_tokens).c_str());

        write_file((get_bin_dir() + "/shaders/lang_tokens_generated").c_str(),
            shader_token_defs(c_tokens).c_str());

        write_file((get_bin_dir() + "/shaders/ast_tokens_generated").c_str(),
            yacc_parse_tree.to_shader_defs().c_str());

        write_file((get_bin_dir() + "/shaders/phys_registers_generated").c_str(),
            generate_shader_definitions().c_str());

        Shaders shaders = Gl::compile_shaders();

        compile(job, shaders, yacc_parse_tree, ir_parse_tree, lang_tokens_parse_tree, ir_tokens, ast_ssbos, inst_sel);
    } catch (Exception& e) {
        e.print();
        return 1;
    }
    return 0;
}
