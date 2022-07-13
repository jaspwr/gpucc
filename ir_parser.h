#pragma once
#include "inc.h"
#include "simple_token_parser.h"
#include "token_tree_gen.h"
namespace ir_codegen {

	extern int* SSBO; // is malloced and freed in lang
	extern int current_SSBO_length;

	class ir_bitcode_constant {
	public:
		inline static int self_count = 0;
		inline static ir_bitcode_constant* parse_list[256];
		inline static int parsable_count;

		int val;
		std::string string;
		ir_bitcode_constant(const char* str, bool passable) {
			string = (std::string)str;
			val = self_count++;
			//SSBO[current_SSBO_length] = val;
			//current_SSBO_length++;
			if (passable) {
				parse_list[parsable_count] = this;
				parsable_count++;
			}
		}
	};
	
	extern void process_token(char* token, bool in_quotes, token_tree* main_tt);
	extern void finalise_token(int index);
}