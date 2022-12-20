#pragma once
#include "inc.h"
#include "token_tree_gen.h"
#include "lang.h"
#include "ir_parser.h"
#include "simple_token_parser.h"
#include "utils.h"
class yacc_parser{
public:
	inline static simple_token_parser tokens;
private:
	yacc_parser() {};
};
token_tree gen_tree(token_tree* main_tt, int normal_token_count);