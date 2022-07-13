#include "ir_parser.h"



namespace ir_codegen{
#define BUFFER_MAX_SIZE 256
	enum contexts {
		condition,
		instruction
	};
	int* SSBO;
	int current_SSBO_length = 256;

	int instruction_buffer[BUFFER_MAX_SIZE];
	int instruction_buffer_length = 0;
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
	ir_bitcode_constant SELF_REGISTER("%X", NOT_PARSABLE);
	ir_bitcode_constant LANGUAGE_TOKEN_IDENTIFIER("\"", NOT_PARSABLE);
#undef PASSABLE
#undef NON_PASSABLE
	simple_token_parser tokens(ir_bitcode_constant::self_count);


	void append_to_buffer(int value) {
		if (instruction_buffer_length >= BUFFER_MAX_SIZE)
			throw "instruction length exceeded max buffer size";
		instruction_buffer[instruction_buffer_length] = value;
		instruction_buffer_length++;
	}


	int simple_int_parse(char* str) {
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
	void process_token(char* token, bool in_quotes, token_tree* main_tt) {
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


		if (in_quotes) {
			append_to_buffer(LANGUAGE_TOKEN_IDENTIFIER.val);
			append_to_buffer(parse_token_with_tree(main_tt, token));
		}else if (token[0] == '%') {
			append_to_buffer(SELF_REGISTER.val);
		}
		else if (token[0] == '$') {
			if(token[1] == 't') // string length will always be >= 2
				append_to_buffer(REFERENCE_IMPLICIT_TYPE_IDENTIFIER.val);
			else
				append_to_buffer(REFERENCE_IDENTIFIER.val);
			append_to_buffer(simple_int_parse(token));
		}
		else if (token[0] == '!'){
			if (token[1] == 't')
				append_to_buffer(INSERSION_IMPLICIT_TYPE_IDENTIFIER.val);
			else
				append_to_buffer(INSERSION_IDENTIFIER.val);
			append_to_buffer(simple_int_parse(token));
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
					if (simple_token_parser::str_match(
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
			append_to_buffer(END_INSTRUCTION.val);
			instruction_buffer[pre_end_condition_location] = instruction_buffer_length;
			context = contexts::condition;
		}
		else {
		other_tokens:
			append_to_buffer(tokens.get_ab_token_index(token));
		}
	}

	
	void finalise_token(int index) {
		append_to_buffer(CODEGEN_END.val);

		// Bitcode in SSBO format
		//		[0 -> 255]   : Addresses of codegen segment, the index is the token
		//		[256 -> 256+contants_amount] : Constants
		//		[256+contants_amount -> EOF] : Codegen segments
		SSBO[index] = current_SSBO_length; // points to the first entry of codegen segment
		
		// Note: May potentially be a good idea to scrap the buffer and just make it
		//		 write directly to the SSBO array to avoid having to do this copying
		for (int i = 0; i < instruction_buffer_length; i++) {
			SSBO[current_SSBO_length + i] = instruction_buffer[i];
		}

		current_SSBO_length += instruction_buffer_length;
		instruction_buffer_length = 0;
		context = contexts::condition;
	}
}