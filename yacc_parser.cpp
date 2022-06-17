#include "yacc_parser.h"

const char *yacc = R"(
meoddw
	: '_Static_assert' '(' ';' 
	| 'int' 'for' 'auto' '(' 'for' ')'
	;

meow
	: meoddw 'int'
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
			
            str[i] = sentance_buffer[i];
        }
		printf("fuc%ifuck\n", replaces_with);
		str[in_sentance_index] = '\0';
        add_token(str,replaces_with);
    }
    in_sentance_index = 0;
}

token_tree gen_tree(token_tree* main_tt, int normal_token_count){
    flush_tree(true);
	ab_token_counter = normal_token_count;
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
                        }else{
							not_marker = true;
						}
                        break;
                    case '|':
                        if(context != contexts::norm_token)
                        {
                            context = contexts::_sentance;
                            new_sentace();
                        }else{
							not_marker = true;
						}
                        break;
                    case ';':
                        if(context != contexts::norm_token)
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
						printf("CUNT: %i\n",token);
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
						printf("%i\n", main_tt->data[___row].items[substr[__i]].x_jump);
                        sentance_buffer[in_sentance_index] = main_tt->data[___row].items[substr[__i]].x_jump;
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