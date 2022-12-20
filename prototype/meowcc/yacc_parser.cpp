#include "yacc_parser.h"
//TODO: Add comments


const char *interal_yacc = R"(
STRING_LITERAL
    : '"' ... '"'
    ;

CONSTANT
    : CONSTANT '.' CONSTANT
	;

variable_name_read
    : $0 IDENTIFIER
    ; < %X = load $0 #newline >

array_call
    : $1 primary_expression '[' $0 primary_expression ']'
    ; < %X = getelementptr inbounds IMPLICITTYPE , IMPLICITTYPE * $1, i64 0, i64 $0 #newline >

array_call_loaded
    : $0 array_call
    ; < %X = load IMPLICITTYPE , IMPLICITTYPE * $0 #newline >

array_call_write
    : $1 primary_expression '[' $0 primary_expression ']' assignment_operator
    ; < %X = getelementptr inbounds IMPLICITTYPE , IMPLICITTYPE * $1, i64 0, i64 $0 #newline >


primary_expression
    : $0 CONSTANT
    | $0 variable_name_read
	| $0 STRING_LITERAL
    | $0 unary_operation
    | '(' $0 primary_expression ')'
    | $0 expression
    | $0 unary_bool_not
    | $0 unary_bool_negation
    | $0 unary_minus
    | $0 unary_increment
    | $0 unary_decrement
    | $0 ternary_conditional_expression
    | $0 mutliplication
    | $0 division
    | $0 modulo
    | $0 addition
    | $0 subtraction
    | $0 bitshift_left
    | $0 bitshift_right
    | $0 less_than
    | $0 less_than_or_equal_to
    | $0 greater_than
    | $0 greater_than_or_equal_to
    | $0 equality
    | $0 inequality
    | $0 logical_and
    | $0 logical_or
    | $0 logical_xor
    | $0 logical_not
    | $0 bitwise_and
    | $0 bitwise_or
    | $0 bitwise_xor
    | $0 function_whole
    | $0 function_whole_no_args
    | $0 array_call_loaded
	;

scope
    : $0 start_connected_terminating_expression '}'
    | '{' '}'
    ; < Xl #newline>

assignment
    : $1 variable_name_write $0 primary_expression ';'
    | $1 array_call_write $0 primary_expression ';'
    ; <store i32 $0.0, i32 * $1 #newline>

unary_bool_not
    :  { ')', primary_expression} '!' $0 primary_expression
    ; < %X = xor i1 $0.0, true #newline >

unary_bitwise_not
    :  { ')', primary_expression} '~' $0 primary_expression
    ; < %X = xor i32 $0.0, -1 #newline >

unary_minus
    :  { ')', primary_expression} '-' $0 primary_expression
    ; < %X = sub nsw i32 0, $0.0 #newline >

unary_increment
    : $0 primary_expression '++'
    ; < %X = add nsw i32 $0.0, 1 #newline >

unary_decrement
    : $0 primary_expression '--'
    ; < %X = sub nsw i32 $0.0, 1 #newline >










variable_name_write
    : $0 CONSTANT '='
    ; < %X = load ptr $0 #newline >



addition
    : [ '*', '/', '%'} $0 primary_expression '+' $1 primary_expression
    ; < %X = add nsw i32 $0.0, $1.0 #newline >

subtraction
    : [ '*', '/', '%' } $0 primary_expression '-' $1 primary_expression
    ; < %X = sub nsw i32 $0.0, $1.0 #newline >

mutliplication
    : $0 primary_expression '*' $1 primary_expression
    ; < %X = mul nsw i32 $0.0, $1.0 #newline >

division
    : $0 primary_expression '/' $1 primary_expression
    ; < %X = sdiv i32 $0.0, $1.0 #newline >

modulo
    : $0 primary_expression '%' $1 primary_expression
    ; < %X = srem i32 $0.0, $1.0 #newline >



logical_greater_than
    : [ '*', '/', '%', '-', '+', '<<', '>>'} $0 primary_expression '>' $1 primary_expression
    ; < %X = icmp sgt i32 $0.0, $1.0 #newline >

logical_less_than
    : [ '*', '/', '%', '-', '+', '<<', '>>'} $0 primary_expression '<' $1 primary_expression
    ; < %X = icmp slt i32 $0.0, $1.0 #newline >

logical_greater_than_or_equal
    : [ '*', '/', '%', '-', '+', '<<', '>>'} $0 primary_expression '>=' $1 primary_expression
    ; < %X = icmp sge i32 $0.0, $1.0 #newline >

logical_less_than_or_equal
    : [ '*', '/', '%', '-', '+', '<<', '>>' } $0 primary_expression '<=' $1 primary_expression
    ; < %X = icmp sle i32 $0.0, $1 #newline >

logical_comparison_expression
    : $0 logical_greater_than
    | $0 logical_less_than
    | $0 logical_greater_than_or_equal
    | $0 logical_less_than_or_equal
    ; < %X = zext i1 $0 to i32 #newline >


ternary_condition
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=', '==', '!=', '&', '^', '|', '&&', '||' } $0 primary_expression '?'
    ; < %X = icmp ne i32 $0.0, 0 #newline >

ternary_conditional_expression
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=', '==', '!=', '&', '^', '|', '&&', '||', '?' } $0 ternary_condition $1 primary_expression ':' $2 primary_expression
    ; <%X = select i1 $0, $1.0, $2.0 #newline >



logical_equality
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=' } $0 primary_expression '==' $1 primary_expression
    ; < %X = icmp eq i32 $0.0, $1.0 #newline >

logical_inequality
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=' } $0 primary_expression '!=' $1 primary_expression
    ; < %X = icmp ne i32 $0.0, $1.0 #newline >

logical_equality_expression
    : $0 logical_equality
    | $0 logical_inequality
    ; < %X = zext i1 $0 to i32 #newline >


logical_and
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=', '==', '!=', '&', '^', '|' } $0 primary_expression '&&' $1 primary_expression
    ; < %X = gwaaaaaaaa #newline >

logical_or
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=', '==', '!=', '&', '^', '|', '&&'} $0 primary_expression '||' $1 primary_expression
    ; < %X = meowmoewmoew #newline >


bitshift_left
    : [ '*', '/', '%', '-', '+'} $0 primary_expression '<<' $1 primary_expression
    ; < %X = shl i32 $0.0, $1.0 #newline >

bitshift_right
    : [ '*', '/', '%', '-', '+' } $0 primary_expression '>>' $1 primary_expression
    ; < %X = shr i32 $0.0, $1.0 #newline >

bitwise_xor
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=', '==', '!=', '&' } $0 primary_expression '^' $1 primary_expression
    ; < %X = xor i32 $0.0, $1.0 #newline >

bitwise_and
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=', '==', '!=' } $0 primary_expression '&' $1 primary_expression
    ; < %X = and i32 $0.0, $1.0 #newline >

bitwise_or
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=', '==', '!=', '&', '^' } $0 primary_expression '|' $1 primary_expression
    ; < %X = or i32 $0.0, $1.0 #newline >




start_connected_terminating_expression
    : '{' $0 primary_expression ';'
    | $0 start_connected_terminating_expression $1 primary_expression ';'
    | '{' $0 assignment
    | start_connected_terminating_expression $0 assignment
    | '{' $0 full_branch
    | start_connected_terminating_expression $0 full_branch
    ;

ret
    : 'return' $0 primary_expression ';'
    ; < ret i32 $0.0.0 #newline >
)";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum contexts{
    ab_tokens,
    _sentance,
    norm_token,
    exclusion_tokens,
    codegen
};
contexts context = contexts::ab_tokens;

int working_row = 0;
int max_row = 0;

int catergorise(char x){
    //TODO: make this not disgusting
    if(x == '\'')
        return 2;
    if(context == contexts::norm_token)
        return 6;
    if(x < '!')
        return 1;
    if(x == '|')
		return 3;
	if(x == ':')
        return 4;
	if(x == '.')
        return 5;
    if (x == '{')
        return 7;
    if (x == '}')
        return 8;
    if (x == '<')
        return 9;
    if (x == '>')
        return 10;
    if (x == ',')
        return 11;
    if (x == ';')
        return 12;
    return 0;
}


int in_sentance_index = 0;
int in_exlusion_tokens_index = 0;
int replaces_with = 0;
int parse_tree_extra_pointer = 0;
char sentance_buffer[10];
int operand_order_buffer[10];
int pre_token_exclusions_buffer[30];
bool symetrical_exclusion;
contexts context_returning_to;
void new_sentace(){
    if(in_sentance_index != 0){
        char* str = (char*)malloc(in_sentance_index+1); // Freed in token_tree_gen

        parse_tree_extra new_entry_data;
        for(int i = 0; i < in_sentance_index; i++){
            new_entry_data.operand_order[i] = operand_order_buffer[i];
            str[i] = sentance_buffer[i];
            operand_order_buffer[i] = 0;
        }
        for (int i = 0; i < in_exlusion_tokens_index; i++) {
            new_entry_data.pre_char_exclusions[i] = pre_token_exclusions_buffer[i];
        }
        new_entry_data.pre_char_exclusions_counter = in_exlusion_tokens_index;
        new_entry_data.symetrical_exclusion = symetrical_exclusion;


        //check if identical entry in pass_tree_extra_list_exists
        bool new_entry = true;
        int entry_index;
        for (entry_index = 0;
            entry_index < parse_tree_extra_pointer;
            entry_index++) {
            if (lang::_parse_tree_extra[entry_index] == new_entry_data) {
                new_entry = false;
                break;
            }
        }
        if (new_entry) {
            lang::_parse_tree_extra[parse_tree_extra_pointer] = new_entry_data;
            entry_index = parse_tree_extra_pointer;
        }

        str[in_sentance_index] = '\0';
        add_token(str,replaces_with, entry_index);
        free(str);
        if(new_entry)
            parse_tree_extra_pointer++;
    }
    in_sentance_index = 0;
    in_exlusion_tokens_index = 0;

}

int operand_order = 0;
int operand_order_current() {
    int ret = operand_order;
    operand_order = 0;
    return ret;
}


token_tree gen_tree(token_tree* main_tt, int normal_token_count) {
    string yacc = "";
    if(compiler::custom_yacc != nullptr) {
        yacc = utils::load_file_utf8(compiler::custom_yacc->c_str());
    }
    else {
        yacc = std::string(interal_yacc);
    }
    lang::_parse_tree_extra = (parse_tree_extra*)malloc(sizeof(parse_tree_extra) * 255);
    flush_tree(true);
    yacc_parser::tokens.ab_token_counter = normal_token_count;
    yacc_parser::tokens.starting_ab_token_counter = yacc_parser::tokens.ab_token_counter;
    int substr_len = 1;
    int token_start = 0;
    bool white_space = true;
    #define MAX_LENGTH 10000
    for(int i = 0; i < MAX_LENGTH && yacc[i] != '\0'; i++){
        //if at least one character in token is not an empty char set to false
        if(yacc[i] > ' ' && (yacc[i] != ',' || context == contexts::codegen))
            white_space = false;

        if(catergorise(yacc[i+1]) != catergorise(yacc[i])){
            if(substr_len != 0 && !white_space){
                //run when 'i' reaches end of a token
                char *substr = (char*)malloc(substr_len + 1);
                for(int ii = 0; ii < substr_len; ii++){
                    substr[ii] = yacc[token_start + ii];
                }
                substr[substr_len] = '\0';

                // check for colon, semicolon, ect.
                bool not_marker = true;
                if(substr_len == 1)
                {
                    //TODO: reimplement this to repeat itself less
                    not_marker = false;
                    switch (substr[0])
                    {
                    case ':':
						
                        if(context != contexts::norm_token && context != contexts::codegen)
                        {
							if (context != contexts::ab_tokens)
								throw "Invalid syntax: Unexpected ':'"; // Invalid syntax
                            context = contexts::_sentance;
                            new_sentace();
                        }else{
							not_marker = true;
						}
                        break;
                    case '|':
                        if(context != contexts::norm_token && context != contexts::codegen)
                        {
                            context = contexts::_sentance;
                            new_sentace();
                        }else{
							not_marker = true;
						}
                        break;
                    case ';':
                        if(context != contexts::norm_token && context != contexts::codegen)
						{
                            context = contexts::ab_tokens;
							new_sentace();
						}else{
							not_marker = true;
						}
                        break;
                    case '\'':
                        //escape to sentance or begin new 'in quotes' block
                        if(context == contexts::norm_token)
                            context = context_returning_to;
                        else {
                            context_returning_to = context;
                            context = contexts::norm_token;
                        }
                        break;
                    case '{':
                        if (context != contexts::norm_token && context != contexts::codegen) {
                            context = contexts::exclusion_tokens;
                            symetrical_exclusion = false;
                        }
                        else
                            not_marker = true;
                        break;
                    case '[':
                        if (context != contexts::norm_token && context != contexts::codegen) {
                            context = contexts::exclusion_tokens;
                            symetrical_exclusion = true;
                        }
                        else
                            not_marker = true;
                        break;
                    case '}':
                        if (context != contexts::norm_token && context != contexts::codegen){ 
                            context = contexts::_sentance;
                        }
                        else
                            not_marker = true;
                        break;
                    case '<':
                        if (context != contexts::norm_token && context != contexts::codegen)
                            context = contexts::codegen;
                        else
                            not_marker = true;
                        break;
                    case '>':
                        if (context != contexts::norm_token){
                            context = contexts::ab_tokens;
                            ir_codegen::finalise_token(replaces_with);
                        }
                        else
                            not_marker = true;
                        break;
                    default:
                        not_marker = true;
                        break;
                    }
                }
                if (not_marker) {
                    if (context == contexts::codegen 
                        || (context == contexts::norm_token 
                            && context_returning_to == contexts::codegen)) {
                        ir_codegen::process_token(substr, context == contexts::norm_token, main_tt);
                    }
                    else {
                        // printf("%s\n", substr);
                        // Handle special tokens (TODO: implement this a neater way)
                        //
                        //      "..." -> ANYTHING_TOKEN
                        //      "IDENTIFIER" -> ANY_IDENTIFIER_TOKEN
                        //      "CONSTANT" -> ANY_LITERAL_TOKEN
                        //      
                        //      $X puts the following token as position X in the child nodes when 
                        //      constructing the ast.
                        //
                        if (utils::str_match(substr, (char*)"...")) {
                            sentance_buffer[in_sentance_index] = ANYTHING_TOKEN;
                            operand_order_buffer[in_sentance_index] = operand_order_current();
                            in_sentance_index++;
                        }
                        else if (substr[0] == '$') {
                            //TODO implement with proper int parse function
                            operand_order = (int)(substr[1] - '0') + 2;
                        }
                        else if (utils::str_match(substr, (char*)"IDENTIFIER")) {
                            sentance_buffer[in_sentance_index] = ANY_IDENTIFIER_TOKEN;
                            operand_order_buffer[in_sentance_index] = operand_order_current();
                            in_sentance_index++;
                        }
                        else if (utils::str_match(substr, (char*)"CONSTANT")) {
                            sentance_buffer[in_sentance_index] = ANY_LITERAL_TOKEN;
                            operand_order_buffer[in_sentance_index] = operand_order_current();
                            in_sentance_index++;
                        }
                        else {
                            int token = 0;
                            switch (context)
                            {
                            case contexts::ab_tokens:
                                token = yacc_parser::tokens.get_ab_token_index(substr);
                                replaces_with = token;
                                //printf("%i -> ", token);
                                //printf("%s\n", substr);
                                break;
                            case contexts::_sentance:
                                token = yacc_parser::tokens.get_ab_token_index(substr);
                                sentance_buffer[in_sentance_index] = (char)token;
                                operand_order_buffer[in_sentance_index] = operand_order_current();
                                in_sentance_index++;
                                break;
                            case contexts::exclusion_tokens:
                                token = yacc_parser::tokens.get_ab_token_index(substr);
                                pre_token_exclusions_buffer[in_exlusion_tokens_index] = (int)token;
                                in_exlusion_tokens_index++;

                                break;
                            case contexts::norm_token:
                                token = parse_token_with_tree(main_tt, substr);
                                if (token == 0)
                                    throw "Unkown token in YACC"; // Token did not parse
                                //printf("%i\n", token);
                                char _ret = (char)token;
                                if (context_returning_to == contexts::exclusion_tokens) {
                                    pre_token_exclusions_buffer[in_exlusion_tokens_index] = token;
                                    in_exlusion_tokens_index++;
                                }
                                else {
                                    sentance_buffer[in_sentance_index] = _ret; // Note: Implicite cast from uchar to char 
                                                                               //       which may cause issues.
                                    operand_order_buffer[in_sentance_index] = operand_order_current();

                                    in_sentance_index++;
                                }
                                break;
                            }
                        }
                    }
                }
                free(substr);
            }

            substr_len = 1;
            token_start = i+1;
            white_space = true;
        }else{
            substr_len++;
        }
    }
    #undef MAX_LENGTH
    

	return token_tree_gen();
}