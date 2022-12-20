#include "ir_parser.h"
#include "compiler.h"


namespace ir_codegen{
#define BUFFER_MAX_SIZE 512
	enum contexts {
		condition,
		instruction
	};
	int* SSBO;
	int current_SSBO_length = 256;

	int instruction_buffer[BUFFER_MAX_SIZE];
	int instruction_buffer_length = 0;
	int longest_instruction = 0;
	int instruction_length_counter = 0;
	contexts context = contexts::condition;
	
#define PARSABLE true
#define NOT_PARSABLE false
	ir_bitcode_constant CODEGEN_END("EOF", NOT_PARSABLE);
	ir_bitcode_constant DEFAULT_CONDITION("defaut", PARSABLE);
	ir_bitcode_constant CONDITIAL_EQAULS_TO("==", PARSABLE);
	ir_bitcode_constant END_CONDITION(":", NOT_PARSABLE);
	ir_bitcode_constant END_INSTRUCTION(";", NOT_PARSABLE);
	ir_bitcode_constant INTERNAL_ELEMENT(".", PARSABLE);
	ir_bitcode_constant REFERENCE_IDENTIFIER("$", NOT_PARSABLE);
	ir_bitcode_constant REFERENCE_IMPLICIT_TYPE_IDENTIFIER("$t", NOT_PARSABLE);
	ir_bitcode_constant INSERSION_IDENTIFIER("!", NOT_PARSABLE);
	ir_bitcode_constant INSERSION_IMPLICIT_TYPE_IDENTIFIER("!t", NOT_PARSABLE);
	ir_bitcode_constant SELF_REGISTER_IDENTIFIER("%", NOT_PARSABLE);
	ir_bitcode_constant SELF_REGISTER_VALUE("X", NOT_PARSABLE);
	ir_bitcode_constant SELF_LABEL_IDENTIFIER("X:", NOT_PARSABLE);
	ir_bitcode_constant LANGUAGE_TOKEN_IDENTIFIER("\"", NOT_PARSABLE);
	ir_bitcode_constant NEWLINE("#newline", PARSABLE);
	ir_bitcode_constant IDENTIFIER_IDENTIFIER("IDENIFIER", NOT_PARSABLE);
#undef PASSABLE
#undef NON_PASSABLE
	simple_token_parser tokens(ir_bitcode_constant::self_count);


	void append_to_buffer(int value) {
		printf("PUSHED: %i\n", value);
		if (instruction_buffer_length >= BUFFER_MAX_SIZE)
			throw "instruction length exceeded max buffer size";
		instruction_buffer[instruction_buffer_length] = value;
		instruction_buffer_length++;
	}


	int simple_int_parse(char* str) {
		// For parent reference
		if (str[0] == '^') {
			return -2;
		}
		// Always skips the first char and assumes that 'str' will always have a terminating null char
		// Faster than dealing with substrings and then standard library parse functions
		int l = 1;
		while (str[l] != '\0') l++;
		int ret = 0;
		int ten_multiple = 1;
		while (l > 1) {
			l--;
			ret += (str[l] - '0') * ten_multiple;
			ten_multiple *= 10;
		}
		return ret;
	}

	int pre_end_condition_location;
	int int_parse_next_token = false;
	void process_token( char* token,
						bool in_quotes, 
						token_tree* main_tt) {
		// TOKEN TYPES
		//		'%A =' -> assigning to new regester
		//		$X -> reference to child node (instruction was called previously)
		//		!X -> insersion of child node (contents of node's parsed IR placed in position
		//									   of token; instruction not called previously.
		//		$tX -> implicit type of child node (i1, i32, i32*, label, ect.)
		//		$X.$Y | !X.$Y | $tX.$Y -> refers to child node of child node, can go deeper if needed
		//		'A:' -> new basic block label
		// 
		//		All other tokens are parsed to a hard coded value. Types can also be written explicitly
		//		and will be parsed this way. Variable assigns should always be pared to "store" and
		//		expressions should use registers of the previous fucntions
		//
		if (context == contexts::instruction)
			instruction_length_counter++;


		if (in_quotes) {
			append_to_buffer(LANGUAGE_TOKEN_IDENTIFIER.val);
			append_to_buffer(parse_token_with_tree(main_tt, token));
			printf("TOKEN %s\n", token);
		}
		else if (int_parse_next_token) {
			int_parse_next_token = false;
			append_to_buffer(simple_int_parse(token) + 1);
			printf("int %s\n", token);

		}
		else if (token[0] == '%') {
			append_to_buffer(SELF_REGISTER_IDENTIFIER.val);
			append_to_buffer(SELF_REGISTER_VALUE.val);
			printf("%\n");

		}
		else if (utils::str_match(token, (char*)"Xl")) {
			append_to_buffer(SELF_LABEL_IDENTIFIER.val);
			append_to_buffer(SELF_REGISTER_VALUE.val);
			printf("LAB\n");

		}
		else if (token[0] == '$') {
			//if(token[1] == 't') // string length will always be >= 2
			//	append_to_buffer(REFERENCE_IMPLICIT_TYPE_IDENTIFIER.val);
			//else
			append_to_buffer(REFERENCE_IDENTIFIER.val);
			append_to_buffer(simple_int_parse(token)+1);
			printf("$%s\n", token);

		}
		else if (token[0] == '!'){
			if (token[1] == 't')
				append_to_buffer(INSERSION_IMPLICIT_TYPE_IDENTIFIER.val);
			else
				append_to_buffer(INSERSION_IDENTIFIER.val);
			append_to_buffer(simple_int_parse(token)+1);
			printf("NO HERE 1\n");
		}
		else if (context == contexts::condition) {

			if (token[0] == ':') {
				context = contexts::instruction;
				append_to_buffer(END_CONDITION.val);
				pre_end_condition_location = instruction_buffer_length;
				append_to_buffer(0); // Is replaced when end of instruction is known
			}else{
				bool found_token_flag = false;
				for (int i = 0; i < ir_bitcode_constant::parsable_count; i++)
				{
					if (utils::str_match(
						token,(char*)ir_bitcode_constant::parse_list[i]->string.c_str())) {
						append_to_buffer(ir_bitcode_constant::parse_list[i]->val);
						found_token_flag = true;
						break;
					}
				}
				if (!found_token_flag)
					goto other_tokens; // shut up
			}
		}
		else if (context == contexts::instruction && token[0] == ';') {
			if (instruction_length_counter > longest_instruction)
				longest_instruction = instruction_length_counter;
			instruction_length_counter = 0;
			append_to_buffer(END_INSTRUCTION.val);
			instruction_buffer[pre_end_condition_location] = instruction_buffer_length;
			context = contexts::condition;
		}
		else {
		other_tokens:
			printf("OTHER TOKEN: %s\n", token);
			append_to_buffer(tokens.get_ab_token_index(token));
			printf("was assigned: %i\n", tokens.get_ab_token_index(token));
		}

		if (utils::str_match(token, (char*)".")) {
			int_parse_next_token = true;
		}
	}

	
	void finalise_token(int index) {
		append_to_buffer(CODEGEN_END.val);

		// SSBO format
		//		[0 -> 255]   : Addresses of codegen segment, the index is the token
		//		[256 -> 256+contants_amount] : Constants
		//		[256+contants_amount -> EOF] : Codegen segments
		SSBO[index] = current_SSBO_length; // points to the first entry of codegen segment
		
		if (instruction_length_counter > longest_instruction)
			longest_instruction = instruction_length_counter;
		if (longest_instruction < 1)
			longest_instruction = 1;
		SSBO[current_SSBO_length] = instruction_buffer_length; // change to longest_instruction eventually
		longest_instruction = 0;
		instruction_length_counter = 0;
		current_SSBO_length++;
		// Note: May potentially be a good idea to scrap the buffer and just make it
		//		 write directly to the SSBO array to avoid having to do this copying
		for (int i = 0; i < instruction_buffer_length; i++) {
			SSBO[current_SSBO_length + i] = instruction_buffer[i];
			printf("COPIED: %i", instruction_buffer[i]);
		}
		printf("\n");


		current_SSBO_length += instruction_buffer_length;
		instruction_buffer_length = 0;
		context = contexts::condition;
	}
	
	const char* fetch_token_from_source(int location) {
		int max_len = strnlen_s(source, 600000); // NOTE: Temp limit

		int len = compiler::token_length(source, location);
		char* ret = nullptr;
		if (location > max_len || location < 0) {
			ret = new char[4];
			memcpy(ret, "iir", 4);
		} else {
			ret = new char[len + 1];
			memcpy(ret, source + location, len);
			ret[len] = '\0';
		}
		return ret;
	}

	int* register_reduction_buffer;
#define REGISTER_REDUCTION_BUFFER_SIZE 10000
	int real_register_count = 0;
	int get_final_register(int fake_register) {
		// This is scuffed don't do this fixed buffer stuff.
		if(fake_register > REGISTER_REDUCTION_BUFFER_SIZE) {
			print(PRINT_ERROR, "Register issue.");
			throw;
		}
		if (register_reduction_buffer[fake_register] == 0) {
			register_reduction_buffer[fake_register] = real_register_count == 0 ? -1 : real_register_count;
			real_register_count++;
		}
		auto ret = register_reduction_buffer[fake_register];
		if (ret == -1) { // For reg %0
			ret = 0;
		}
		return ret;
	}

	void dbg_printing(int* outp, int len) {
		int counter = 0;
		register_reduction_buffer = new int[REGISTER_REDUCTION_BUFFER_SIZE] { 0 };
		for (int i = 0; i < len; i++) {
			switch (compiler::debugging_config) {
			case compiler::_debugging_config::ir_printout:
			{
				int ind = utils::_char_designed((int)outp[i]);
				if (!ir_codegen::tokens.token_list[ind].empty()) {
					std::cout << ir_codegen::tokens.token_list[ind] << " ";
				}
				else {
					if (ind == NEWLINE.val) {
						std::cout << "\n";
					}
					else if (ind == REFERENCE_IDENTIFIER.val) {
						int v = outp[i + 1];
						if (v > 0) {
							std::cout << "%" << get_final_register(outp[i + 1]) << " ";
						}
						else {
							const char* token = fetch_token_from_source(-v);
							std::cout << token << " ";
							delete[] token;
						}
						i++;
					}
					else if (ind == SELF_REGISTER_IDENTIFIER.val) {
						std::cout << "%" << get_final_register(outp[i + 1]) << " ";
						i++;
					}
					else if (ind == SELF_LABEL_IDENTIFIER.val) {
						std::cout << get_final_register(outp[i + 1]) << ":" << " ";
						i++;
					}
					else if (ind == IDENTIFIER_IDENTIFIER.val) {
						// Unused
						const char* token = fetch_token_from_source(outp[i + 1]);
						std::cout << token << " ";
						delete[] token;
						i++;
					}
					else if(ind != 0){
						printf("BROKEN TOKEN: %i\n", outp[i]);
					}
				}
				break;
			}
			case compiler::_debugging_config::ast_nodes_printout:
				if (outp[i] > 90 && outp[i] < yacc_parser::tokens.ab_token_counter && !yacc_parser::tokens.token_list[outp[i]].empty())
					cout << yacc_parser::tokens.token_list[outp[i]] << '\n';
				break;
			case compiler::_debugging_config::tokens_printout:
				switch (counter)
				{
				case 0:
					if (outp[i] > 0 && outp[i] < yacc_parser::tokens.ab_token_counter && !yacc_parser::tokens.token_list[outp[i]].empty())
						cout << yacc_parser::tokens.token_list[outp[i]] << '\n';
					break;
				case 1:
					cout << outp[i] << '\n';
					break;
				}

				counter++;
				if (counter > 2)
					counter = 0;
				break;
			}
		}
	}

	void post_process(int* outp) {
		if (compiler::debugging_config != compiler::_debugging_config::none) {
			printf(";DBG_OUTPUT_START");
			dbg_printing(outp, 5000);
			printf(";DBG_OUTPUT_END");
		}
		delete[] register_reduction_buffer;
		printf("\n");
	}
}