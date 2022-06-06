#include "token_tree_gen.cpp"

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
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
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
	: TYPEDEF
	| EXTERN
	| STATIC
	| AUTO
	| REGISTER
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

int catergorise(char x){
    if(x < 0x21)
        return 1;
    if(x == '\'')
        return 2;
    if(x == '|' | ':' | ';')
        return 3;
    return 0;
}

struct sentance{
    unsigned char* tokens;
};

struct abstract_token{
    unsigned char identifier;
    sentance* handles;
};

struct cst_rgba_pixel
{
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 0;
};

#define CST_ROW_WIDTH 256
struct cst_row
{
    cst_rgba_pixel vals[CST_ROW_WIDTH];
};


struct cst{
    //abstract_token* abstract_tokens;
    cst_row rows[512];
    int height;
};

enum contexts{
    ab_tokens,
    _sentance,
    norm_token
};

cst yacc_token_tree_gen(token_tree* main_tt){
//int main(){
    flush_tree();
    const int COLON = add_token(":"); // 1
    const int LINE_THING = add_token("|");  // 2
    const int QUOTE = add_token("'"); // 3 
    const int SEMICOLON = add_token(";"); // 4
    cst ret;
    token_tree tt = token_tree_gen();
    int len = strlen(yacc);
    
    unsigned char row = 0;
    bool token_unknown = false;
    int i;
    int start_char = 0;
    contexts context = contexts::ab_tokens;
    int replacment = 0;
    int main_tt_height = 0;
    int in_sentance_index = 0;
    int working_row;
	char *substr__;

    for(i = 0; i < len; i++){
        if(i != 0 && catergorise(yacc[i]) != catergorise(yacc[i - 1])){
            unsigned int t = 0;
            bool white_space = false;
            if(!token_unknown)
                t = rows[row].items[yacc[i-1]].x_jump - 1;
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
                if(!white_space)
                {
                    t = add_token(substr) - 1;
                    tt = token_tree_gen();
                }
				substr__ = substr;
            }
            if(t < 0)
                t = 0;
            row = 0;
            token_unknown = false;
            start_char = i;
            
            
            
            if(!white_space){
                // Deal with tokens
                // Every abstact token created here is going to have tt.height added to it
                //printf("%i\n",t);

                

                //std::cout <<  << std::endl;
                switch (t)
                {
                case 0: // colon
                    if(context != contexts::norm_token)
                    {
                        context = contexts::_sentance;
                        in_sentance_index = 0;
                    }
                    break;                
                case 1:
                    //new sentance
                    if(context != contexts::norm_token)
                    {
                        in_sentance_index = 0;
                        //ret.rows[working_row].vals[in_sentance_index].r = 1;
                    }
                    break;
                case 2: // quote
                    if(context == contexts::norm_token)
                        context = contexts::_sentance;
                    else
                        context = contexts::norm_token;
                    break;
                case 3: //semicolon
                    if(context != contexts::norm_token)
                    {
                        //ret.rows[working_row].vals[in_sentance_index].r = 1;
                        context = contexts::ab_tokens;
                    }
                    break;
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
						printf("asdsddasd");
						for(__i = 0; substr__[__i] != '\0'; __i++){
							//printf("\n%i\n\n",rows[row].items[str[__i]].a);
							___row = scuffed_fix[___row].items[substr__[__i]].a;
							
						}
						__i--;
						printf("\n%i\n\n",(int)(scuffed_fix[___row].items[substr__[__i]].x_jump));
						
					}
                        break;
                    case contexts::_sentance:
                        
                        //std::cout << (int)(t) << " -> " << tokens[replacment].string_  << std::endl;
                        if(in_sentance_index == 0){
                            //std::cout << (int)(t) << " -> " << tokens[t].string_ << std::endl;
                            working_row = t;
                            if(working_row > ret.height)
                                ret.height = working_row;
                            if(working_row < 0)
                                working_row = 0;
                            while(ret.rows[working_row].vals[in_sentance_index].r != 0){
                                in_sentance_index++;
                                if(in_sentance_index > CST_ROW_WIDTH)
                                {
                                    //TODO: fail and die
									throw 1;
                                    break;
                                }
                            }
                            //in_sentance_index--;
                            ret.rows[working_row].vals[in_sentance_index].r = replacment;
                            ret.rows[working_row].vals[in_sentance_index+1].r = 1;

                        }else{
                            //std::cout << "\t" << (int)(t) << " -> " << tokens[t].string_ << std::endl;
                            ret.rows[working_row].vals[in_sentance_index].r = (unsigned char)t;
                            ret.rows[working_row].vals[in_sentance_index+1].r = 1;
                            //std::cout << "\t" << tokens[(unsigned char)t].string_ << std::endl;
                            //std::cout << "\t" << (unsigned int)t << std::endl;
                        }
                        in_sentance_index += 1;
                        break;
                    }
                    break;
                }

            }
        }
        if(!token_unknown)
        {
            row = rows[row].items[yacc[i]].a;
            if(row == 0){
                token_unknown = true;
            }
        }
    }
    return ret;
    for(int iii = 0; iii < 256; iii++){
        int _in_sentance_index = 0;
        std::cout << iii << " -> " << tokens[iii].string_ << std::endl;
        while(ret.rows[iii].vals[_in_sentance_index].r != 0){
            //printf("%s ", tokens[ret.rows[iii].vals[_in_sentance_index].r].string_);
            //printf("%i ", ret.rows[iii].vals[_in_sentance_index].r);
            
            std::cout << "\t" << tokens[ret.rows[iii].vals[_in_sentance_index].r].string_  << std::endl;
            _in_sentance_index++;
            if(_in_sentance_index > CST_ROW_WIDTH)
            {
                //TODO: fail and die
                break;
            }
        }
    }
    //return ret;
    //return 0;
}

