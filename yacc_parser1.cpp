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
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP
	| postfix_expression DEC_OP
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression
	| DEC_OP unary_expression
	| unary_operator cast_expression
	| SIZEOF unary_expression
	| SIZEOF '(' type_name ')'
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
	| shift_expression LEFT_OP additive_expression
	| shift_expression RIGHT_OP additive_expression
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression LE_OP shift_expression
	| relational_expression GE_OP shift_expression
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression
	| equality_expression NE_OP relational_expression
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
	| logical_and_expression AND_OP inclusive_or_expression
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OR_OP logical_and_expression
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
	| MOD_ASSIGN
	| '+='
	| '-='
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
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
	: VOID
	| CHAR
	| SHORT
	| INT
	| LONG
	| FLOAT
	| DOUBLE
	| SIGNED
	| UNSIGNED
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
	: STRUCT
	| UNION
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
	: ENUM '{' enumerator_list '}'
	| ENUM IDENTIFIER '{' enumerator_list '}'
	| ENUM IDENTIFIER
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
	: CONST
	| VOLATILE
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
	| CASE constant_expression ':' statement
	| DEFAULT ':' statement
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
	: IF '(' expression ')' statement
	| IF '(' expression ')' statement ELSE statement
	| SWITCH '(' expression ')' statement
	;

iteration_statement
	: WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement
	| FOR '(' expression_statement expression_statement expression ')' statement
	;

jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';'
	| RETURN ';'
	| RETURN expression ';'
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

enum contexts{
    ab_tokens,
    _sentance,
    norm_token
};
contexts context = contexts::ab_tokens;


int catergorise(char x){
	//this chould be changed to return an enum but it literally just checks if theyre not equal
    if(x < '!')
        return 1;
    if(x == '\'')
        return 2;
	if(context == contexts::norm_token)
		return 0;
    if(x == '|')
		return 3;
	if(x == ':')
        return 4;
	if(x == '&')
        return 5;
    return 0;
}

int in_sentance_index = 0;
unsigned char working_row;
int replacment = 0;

void add_to_working_row(unsigned char t, cst* ret){
	if(in_sentance_index == 0){
		//std::cout << (int)(t) << " -> " << tokens[t].string_ << std::endl;
		working_row = t;
		if(working_row > ret->height)
			ret->height = working_row;
		if(working_row < 0)
			working_row = 0;
		while(ret->rows[working_row].vals[in_sentance_index].r != 0){
			(in_sentance_index) += 1;
			if(in_sentance_index > CST_ROW_WIDTH)
			{
				//TODO: fail and die
				//throw 1;
				break;
			}
		}
		//in_sentance_index--;
		ret->rows[working_row].vals[in_sentance_index].r = replacment;
		ret->rows[working_row].vals[(in_sentance_index)+1].r = 1;

	}else{
		//std::cout << "\t" << (int)(*t) << " -> " << tokens[*t].string_ << std::endl;
		ret->rows[working_row].vals[in_sentance_index].r = (char)t;
		ret->rows[working_row].vals[(in_sentance_index)+1].r = 1;
		//std::cout << "\t" << tokens[(unsigned char)t].string_ << std::endl;
		//std::cout << "\t" << (unsigned int)t << std::endl;
	}
	in_sentance_index += 1;
}


cst yacc_token_tree_gen(token_tree* main_tt){
//int main(){

    flush_tree(true);
	
    const int COLON = add_token(":"); // 1
    const int LINE_THING = add_token("|");  // 2
    const int QUOTE = add_token("'"); // 3 
    const int SEMICOLON = add_token(";"); // 4
	for(int iiii = 0; iiii < main_tt->token_count-4; iiii++)
		add_token(";i");


    cst ret;
    token_tree tt = token_tree_gen();
    int len = strlen(yacc);
    
    unsigned char row = 0;
    bool token_unknown = false;
    int i;
    int start_char = 0;
    int main_tt_height = 0;

	char *substr__;

    for(i = 0; i < len; i++){
		//could be a little better inplemented so that catergorise isnt run twice on the same char
        if(i != 0 && catergorise(yacc[i]) != catergorise(yacc[i - 1])){
            unsigned char t = 0;
            bool white_space = false;
            if(!token_unknown)
                t = tt.data[row].items[yacc[i-1]].x_jump - 1;
            else{
                int _len = i - start_char;
                char substr[_len+1];
                //strncpy(substr, yacc + start_char, _len);
                for(int ___i = 0; ___i < _len; ___i++){
                    substr[___i] = yacc[start_char+___i];
                }
                substr[_len] = '\0';
                for(int ii = 0; substr[ii] != '\0'; ii++){
                    if(substr[ii] < 0x21){
                        white_space = true;
                        break;
                    }
                }
				//printf("%s\n", substr);
                if(!white_space)
                {
                    t = add_token(substr) - 1;
                    tt = token_tree_gen();
                }
				substr__ = substr;
            }

            row = 0;
            token_unknown = false;
            start_char = i;
            
            
            
            if(!white_space){
                // Deal with tokens
                // Every abstact token created here is going to have tt.height added to it
                //printf("%i\n",t);

				if(t < 0)
					t = 0;
                
				// printf("%i\n",t);
                // std::cout << tokens[t].string_ << std::endl;
                switch (t)
                {
                case 0: // colon
                    if(context != contexts::norm_token)
                    {
                        context = contexts::_sentance;
                        in_sentance_index = 0;
                    }else goto DEAFULT_CASE;
                    break;                
                case 1:
                    //new sentance
                    if(context != contexts::norm_token)
                    {
                        in_sentance_index = 0;
                        //ret.rows[working_row].vals[in_sentance_index].r = 1;
                    }else goto DEAFULT_CASE;
                    break;
                case 2: // quote
                    if(context != contexts::norm_token)
                        context = contexts::norm_token;
                    else
                        context = contexts::_sentance;
                    break;
                case 3: //semicolon
                    if(context != contexts::norm_token)
                    {
                        //ret.rows[working_row].vals[in_sentance_index].r = 1;
                        context = contexts::ab_tokens;
                    }else goto DEAFULT_CASE;
                    break;
				DEAFULT_CASE:
                default:
                    switch (context)
                    {
                    case contexts::ab_tokens:
                        //printf("%s\n",tokens[t-1].string_);
                        replacment = t;
                        //std::cout << tokens[replacment].string_ << std::endl;
                        //std::cout << (int)replacment << std::endl;
                        break;
                    case contexts::norm_token:
					{
						unsigned char ___row = 0;
						int __i;
						   // declaring character array
						int n = tokens[t].string_.length();
						char str[n + 1];
						// copying the contents of the
						// string to char array
						strcpy(str, tokens[t].string_.c_str());
						//printf("%s", str);
						//std::cout << tokens[t].string_;
						for(__i = 0; str[__i] != '\0'; __i++){
							//printf("\n\t%i\n",str[__i]);
							___row = main_tt->data[___row].items[str[__i]].a;
						}
						__i--;
						add_to_working_row((main_tt->data[___row].items[str[__i]].x_jump),&ret);
						//printf(" -> %i\n",(int)(main_tt->data[___row].items[str[__i]].x_jump));
						//add_to_working_row(&in_sentance_index,&working_row,
						//				   (int)(main_tt->data[___row].items[str[__i]].x_jump),
						//				   &ret,&replacment);
					}
                        break;
                    case contexts::_sentance:
                        
                        //std::cout << (int)(t) << " -> " << tokens[replacment].string_  << std::endl;
                        add_to_working_row((unsigned char)t,&ret);

                        break;
						
                    }
                    break;
                }

            }
        }
        if(!token_unknown)
        {
            row = tt.data[row].items[yacc[i]].a;
            if(row == 0){
                token_unknown = true;
            }
        }
    }
	free(tt.data);
    return ret;

    //return ret;
	main_tt_height = 22;
    for(int iii = 0; iii < 256; iii++){
        int _in_sentance_index = 0;
        std::cout << iii << " -> " << tokens[iii].string_ << std::endl;
        //std::cout << iii << " -> " << iii<< std::endl;
        while(ret.rows[iii].vals[_in_sentance_index].r != 0){
            //printf("%s ", tokens[ret.rows[iii].vals[_in_sentance_index].r].string_);
            //printf("%i ", ret.rows[iii].vals[_in_sentance_index].r);
            //ret.rows[iii].vals[_in_sentance_index].r += main_tt_height;
            std::cout << "\t" << tokens[ret.rows[iii].vals[_in_sentance_index].r].string_  << std::endl;
            //std::cout << "\t" << (int)ret.rows[iii].vals[_in_sentance_index].r  << std::endl;
            _in_sentance_index++;
            if(_in_sentance_index > CST_ROW_WIDTH)
            {
                //TODO: fail and die
                break;
            }
        }
    }



    //return 0;
}

