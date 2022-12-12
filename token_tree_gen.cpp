#include "token_tree_gen.h"
// This needs a lot of redesigning to work with in conjuection with Yacc

int __i = 0;
token tokens[len];

int parse_token_with_tree(token_tree* tt, char* str) {
    int ___row = 0;
    int __i;
    //printf("\n\t%s\n", substr);
    for (__i = 0; str[__i] != '\0'; __i++) {

        ___row = tt->data[___row].items[str[__i]].a;
        if (___row == 0) {
            //throw err for token not in dictionary
            if (__i == 0) {
                __i++;
                break;
            }
            throw;
        }
    }
    __i--;
    if (__i < 0)
        __i = 0;
    return tt->data[___row].items[str[__i]].x_jump;
}


int add_token(const char* str, int outp, int parse_tree_extra_pointer){
    tokens[__i].outp = outp;
    //printf("%s\n", str);
    // int ind = __i+add;
    // printf("%i\n", ind);
    tokens[__i].string_ = str;
    tokens[__i].parse_tree_extra_pointer = parse_tree_extra_pointer;
    
    //std::cout << tokens[__i].string_;
    __i++;
    return __i;
}

int char_designed(char c) {
    if (c < 0) {
        auto de = (int)c + 256;
        return de;
    }
    return (int)c;
}

int current_capacity = 1000;
const int _current_capacity = 1000;
tree_row *rows;

//tree_row* scuffed_fix;
void flush_tree(bool reset_len){
    if(reset_len)
        __i = 0;
    //tree_row* rows1 = rows;
    tree_row* t = (tree_row*)malloc(_current_capacity*sizeof(tree_row));
    for(int i = 0; i < _current_capacity; i++) {
        for(int c = 0; c < 256; c++){
            t[i].items[c].x_jump = 0;
            t[i].items[c].exit_id = 0;
            t[i].items[c].b = 255;
            t[i].items[c].a = 0;
        }
    }
    rows = t;
    //if(sizeof(&rows) > 32)
        //delete &rows1;
        // //scuffed_fix = rows1; 
}

token_tree token_tree_gen(){
    //add_token("switch");


    const int width = 256;
    int height = 1;
    
    int tree_row_size = sizeof(tree_row);
    //tree_row *rows = (tree_row*) malloc(tree_row_size*current_capacity);

    
    for(int i = 0; i < __i; i++) {
        int current_row = 0;
        int c;
        int pre_current_row;
        //cout << tokens[i].string_ << "\n";
        //cout << tokens[i].outp << "\n";
        for(c = 0; tokens[i].string_[c] != '\0'; c++){
            //unsigned char* x_jump = &(rows[current_row].items[tokens[i].string_[c]].x_jump);
            //printf("%i\n",tokens[i].string_[c]);

            
                if(rows[current_row].items[char_designed(tokens[i].string_[c])].a == 0){
                    rows[current_row].items[char_designed(tokens[i].string_[c])].a = height;
                    if(height > current_capacity){
                        //none of this works
                        current_capacity++;
                        int size = current_capacity*tree_row_size;
                        tree_row* nrows = (tree_row*)malloc(size*sizeof(tree_row));
                        int length = sizeof(rows)/sizeof(rows[0]);    
                        for (int _i = 0; _i < length; _i++) {     
                            nrows[i] = rows[i];
                        }
                        free(rows);
                        rows = nrows;
                    }
                    height++;
                }
                pre_current_row = current_row;
                current_row = rows[current_row].items[char_designed(tokens[i].string_[c])].a;
                if (tokens[i].string_[c+1] == ANYTHING_TOKEN)
                {
                    rows[pre_current_row].items[char_designed(tokens[i].string_[c])+1].exit_id = (unsigned char)tokens[i].string_[c + 2];
                    c++;
                }

                
                //printf("current_row: %i\n", current_row);
            }
        c--;
        rows[current_row].items[char_designed(tokens[i].string_[c])].x_jump = tokens[i].outp;
        rows[current_row].items[char_designed(tokens[i].string_[c])].b = tokens[i].parse_tree_extra_pointer;
    }
    
    unsigned char str[] = "int";
    unsigned char row = 0;
    int ____i;
    for(____i = 0; str[____i] != '\0'; ____i++){
        //printf("\n%i\n\n",rows[row].items[str[__i]].a);
        row = rows[row].items[str[____i]].a;
        
    }
    ____i--;
    //printf("\n%i\n\n",rows[row].items[str[__i]].x_jump);


    token_tree ret;
    ret.data = rows;
    ret.height = height;
    ret.token_count = __i;
    //free(rows);
    return ret;
}
