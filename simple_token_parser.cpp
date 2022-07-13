#include "simple_token_parser.h"




int simple_token_parser::get_ab_token_index(char* s) {
    for (int i = starting_ab_token_counter; i < ab_token_counter; i++) {
        if (str_match(s, _ab_tokens[i])) {
            return i;
        }
    }
    //token is not in list and needs to be added
    int len = strlen(s) + 1;
    _ab_tokens[ab_token_counter] = (char*)malloc(len);
    //lang::add_to_token_list(s, ab_token_counter);
    token_list[ab_token_counter] = (std::string)s;
    for (int i = 0; i < len; i++)
        (_ab_tokens[ab_token_counter])[i] = s[i];
    ab_token_counter++;
    return ab_token_counter - 1;
}

bool simple_token_parser::str_match(char* s1, char* s2) {
#define MAX_LEN 500
    int i;
    for (i = 0; i < MAX_LEN && (s1[i] != '\0'); i++)
    {
        if (s2[i] == '\0' || s1[i] != s2[i])
            return false;
    }
    return (s2[i] == '\0');
#undef MAX_LEN
}