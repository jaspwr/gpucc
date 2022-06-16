#include "yacc_parser.h"

const char *yacc = R"(primary_expression
	: IDENTIFIER
	| CONSTANT
	| STRING_LITERAL
	| '(' expression ')'
	;

postfix_expression
	: primary_expression
	| postfix_expression '[' expression ']'
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression '.' IDENTIFIER
	| postfix_expression '->' IDENTIFIER
	| postfix_expression '++'
	| postfix_expression '--'
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression
	| '++' unary_expression
	| '--' unary_expression
	| unary_operator cast_expression
	| 'sizeof' unary_expression
	| 'sizeof' '(' type_name ')'
	;

unary_operator
	: '&'
	| '*'
	| '+'
	| '-'
	| '~'
	| '!'
	;

cast_expression
	: unary_expression
	| '(' type_name ')' cast_expression
	;

multiplicative_expression
	: cast_expression
	| multiplicative_expression '*' cast_expression
	| multiplicative_expression '/' cast_expression
	| multiplicative_expression '%' cast_expression
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression
	| additive_expression '-' multiplicative_expression
	;

shift_expression
	: additive_expression
	| shift_expression '<<' additive_expression
	| shift_expression '>>' additive_expression
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression '<=' shift_expression
	| relational_expression '>=' shift_expression
	;

equality_expression
	: relational_expression
	| equality_expression '==' relational_expression
	| equality_expression '!=' relational_expression
	;

and_expression
	: equality_expression
	| and_expression '&' equality_expression
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression '&&' inclusive_or_expression
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression '||' logical_and_expression
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	;

assignment_expression
	: conditional_expression
	| unary_expression assignment_operator assignment_expression
	;

assignment_operator
	: '='
	| '*='
	| '/='
	| '%='
	| '+='
	| '-='
	| '<<='
	| '>>='
	| '&='
	| '^='
	| '|='
	;

expression
	: assignment_expression
	| expression ',' assignment_expression
	;

constant_expression
	: conditional_expression
	;

declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';'
	;

declaration_specifiers
	: storage_class_specifier
	| storage_class_specifier declaration_specifiers
	| type_specifier
	| type_specifier declaration_specifiers
	| type_qualifier
	| type_qualifier declaration_specifiers
	;

init_declarator_list
	: init_declarator
	| init_declarator_list ',' init_declarator
	;

init_declarator
	: declarator
	| declarator '=' initializer
	;

storage_class_specifier
	: 'typedef'
	| 'extern'
	| 'static'
	| 'auto'
	| 'register'
	;

type_specifier
	: 'void'
	| 'char'
	| 'short'
	| 'int'
	| 'long'
	| 'float'
	| 'double'
	| 'singed'
	| 'unsigned'
	| struct_or_union_specifier
	| enum_specifier
	| TYPE_NAME
	;

struct_or_union_specifier
	: struct_or_union IDENTIFIER '{' struct_declaration_list '}'
	| struct_or_union '{' struct_declaration_list '}'
	| struct_or_union IDENTIFIER
	;

struct_or_union
	: 'struct'
	| 'union'
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	| type_qualifier specifier_qualifier_list
	| type_qualifier
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;

struct_declarator
	: declarator
	| ':' constant_expression
	| declarator ':' constant_expression
	;

enum_specifier
	: 'enum' '{' enumerator_list '}'
	| 'enum' IDENTIFIER '{' enumerator_list '}'
	| 'enum' IDENTIFIER
	;

enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;

enumerator
	: IDENTIFIER
	| IDENTIFIER '=' constant_expression
	;

type_qualifier
	: 'const'
	| 'volatile'
	;

declarator
	: pointer direct_declarator
	| direct_declarator
	;

direct_declarator
	: IDENTIFIER
	| '(' declarator ')'
	| direct_declarator '[' constant_expression ']'
	| direct_declarator '[' ']'
	| direct_declarator '(' parameter_type_list ')'
	| direct_declarator '(' identifier_list ')'
	| direct_declarator '(' ')'
	;

pointer
	: '*'
	| '*' type_qualifier_list
	| '*' pointer
	| '*' type_qualifier_list pointer
	;

type_qualifier_list
	: type_qualifier
	| type_qualifier_list type_qualifier
	;


parameter_type_list
	: parameter_list
	| parameter_list ',' ELLIPSIS
	;

parameter_list
	: parameter_declaration
	| parameter_list ',' parameter_declaration
	;

parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER
	;

type_name
	: specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
	| '[' ']'
	| '[' constant_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' constant_expression ']'
	| '(' ')'
	| '(' parameter_type_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_type_list ')'
	;

initializer
	: assignment_expression
	| '{' initializer_list '}'
	| '{' initializer_list ',' '}'
	;

initializer_list
	: initializer
	| initializer_list ',' initializer
	;

statement
	: labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

labeled_statement
	: IDENTIFIER ':' statement
	| 'case' constant_expression ':' statement
	| 'default' ':' statement
	;

compound_statement
	: '{' '}'
	| '{' statement_list '}'
	| '{' declaration_list '}'
	| '{' declaration_list statement_list '}'
	;

declaration_list
	: declaration
	| declaration_list declaration
	;

statement_list
	: statement
	| statement_list statement
	;

expression_statement
	: ';'
	| expression ';'
	;

selection_statement
	: 'if' '(' expression ')' statement
	| 'if' '(' expression ')' statement 'else' statement
	| 'switch' '(' expression ')' statement
	;

iteration_statement
	: 'while' '(' expression ')' statement
	| 'do' statement 'while' '(' expression ')' ';'
	| 'for' '(' expression_statement expression_statement ')' statement
	| 'for' '(' expression_statement expression_statement expression ')' statement
	;

jump_statement
	: 'goto' IDENTIFIER ';'
	| 'continue' ';'
	| 'break' ';'
	| 'return' ';'
	| 'return' expression ';'
	;

translation_unit
	: external_declaration
	| translation_unit external_declaration
	;

external_declaration
	: function_definition
	| declaration
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement
	| declarator declaration_list compound_statement
	| declarator compound_statement
	;
)";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum contexts{
    ab_tokens,
    _sentance,
    norm_token
};
contexts context = contexts::ab_tokens;

int working_row = 0;
int max_row = 0;

int catergorise(char x){
	//this chould be changed to return an enum but it literally just checks if theyre not equal
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
	if(x == '&')
        return 5;
    return 0;
}


char * _ab_tokens[400];
int ab_token_counter;
int starting_ab_token_counter;
bool str_match(char* s1, char* s2){
    #define MAX_LEN 500
    for(int i = 0; i < MAX_LEN && s1[i] != '\0'; i++)
    {
        if(s2[i] == '\0' || s1[i] != s2[i])
            return false;
    }
    return true;
    #undef MAX_LEN
}
int get_ab_token_index(char * s){
    for(int i = starting_ab_token_counter; i < ab_token_counter; i++){
        if(str_match(s,_ab_tokens[i])){
            return i;
        }
    }
    //token is not in list and needs to be added
    int len = strlen(s);
    _ab_tokens[ab_token_counter] = (char *)malloc(len);
    for(int i = 0; i < len; i++)
        (_ab_tokens[ab_token_counter])[i] = s[i];
    ab_token_counter++;
    return ab_token_counter-1;
}

int in_sentance_index = 0;
int replaces_with = 0;
char sentance_buffer[10];
void new_sentace(){
    if(in_sentance_index != 0){
        char str[in_sentance_index+1];
        for(int i = 0; i < in_sentance_index; i++){
			printf("%c\n", sentance_buffer[i]);
            str[i] = sentance_buffer[i];
        }
		str[in_sentance_index] = '\0';
        add_token(str,replaces_with);
    }
    in_sentance_index = 0;
}

token_tree gen_tree(token_tree* main_tt){
    flush_tree(true);
	ab_token_counter = main_tt->height;
    starting_ab_token_counter = ab_token_counter;
    int substr_len = 1;
    int token_start = 0;
    bool white_space = true;
    #define MAX_LENGTH 10000
    for(int i = 0; i < MAX_LENGTH && yacc[i] != '\0'; i++){
        //if at least one character in token is not an empty char set to false
        if(yacc[i] > ' ')
            white_space = false;

        if(catergorise(yacc[i+1]) != catergorise(yacc[i])){
            if(substr_len != 0 && !white_space){
                //run when 'i' reaches end of a token
                char substr[substr_len + 1];
                for(int ii = 0; ii < substr_len; ii++){
                    substr[ii] = yacc[token_start + ii];
                }
                substr[substr_len] = '\0';

                // check for colon, semicolon, ect.
                bool not_marker = true;
                if(substr_len == 1)
                {
                    not_marker = false;
                    switch (substr[0])
                    {
                    case ':':
                        if(context != contexts::norm_token)
                        {
                            context = contexts::_sentance;
                            new_sentace();
                        }
                        break;
                    case '|':
                        if(context != contexts::norm_token)
                        {
                            context = contexts::_sentance;
                            new_sentace();
                        }
                        break;
                    case ';':
                        if(context != contexts::norm_token)
                            context = contexts::ab_tokens;
                        break;
                    case '\'':
                        //escape to sentance or begin new 'in quotes' block
                        if(context == contexts::norm_token)
                            context = contexts::_sentance;
                        else
                            context = contexts::norm_token;
                        break;
                    default:
                        not_marker = true;
                        break;
                    }
                }
                if(not_marker){
                    int token = 0;
                    switch (context)
                    {
                    case contexts::ab_tokens:
                        token = get_ab_token_index(substr);
                        replaces_with = token;
                        break;
                    case contexts::_sentance:
                        token = get_ab_token_index(substr);
                        sentance_buffer[in_sentance_index] = (char)token;
                        in_sentance_index++;
                        break;
                    case contexts::norm_token:
                        int ___row = 0;
                        int __i;
                        for(__i = 0; substr[__i] != '\0'; __i++){
							//printf("\n\t%i\n",str[__i]);
							___row = main_tt->data[___row].items[substr[__i]].a;
						}
						__i--;
						token = main_tt->data[___row].items[substr[__i]].x_jump;
						//printf("%s\n", substr);
                        sentance_buffer[in_sentance_index] = main_tt->data[___row].items[substr[__i]].x_jump+128;
                        in_sentance_index++;
                        break;
                    }
                    //printf("%i\n", token);
                }
            }

            substr_len = 1;
            token_start = i+1;
            white_space = true;
        }else{
            substr_len++;
        }
    }
    #undef MAX_LENGTH
    

    for(int i = starting_ab_token_counter; i < ab_token_counter; i++){
        free(_ab_tokens[i]);
    }
	return token_tree_gen();
}