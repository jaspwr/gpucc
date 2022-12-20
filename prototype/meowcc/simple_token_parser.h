#pragma once
#include "inc.h"
class simple_token_parser
{
public:
	char* _ab_tokens[400];
	int ab_token_counter = 0;
	int starting_ab_token_counter = 0;
	std::string token_list[400];
	int get_ab_token_index(char* s);
	simple_token_parser(int starting_index) {
		ab_token_counter = starting_index;
		starting_ab_token_counter = ab_token_counter;
	}
	simple_token_parser() {}
	~simple_token_parser() {
		for (int i = starting_ab_token_counter; i < ab_token_counter; i++) {
			free(_ab_tokens[i]);
		}
	}
};

