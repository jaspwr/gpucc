#include "token_tree_gen.h"
// This needs a lot of redesigning to work with in conjuection with Yacc

int __i = 0;
token tokens[len];

int add_token(const char* str){
    
    tokens[__i].outp = __i+1;
    // printf("%s\n", str);
    // int ind = __i+add;
    // printf("%i\n", ind);
    tokens[__i].string_ = str;
    //std::cout << tokens[__i].string_;
    __i++;
    return __i;
}


int current_capacity = 1000;
const int _current_capacity = 256;
tree_row *rows;

//tree_row* scuffed_fix;
void flush_tree(bool reset_len){
    if(reset_len)
        __i = 0;
    //tree_row* rows1 = rows;
    tree_row* t = (tree_row*)malloc(_current_capacity*sizeof(tree_row));
    for(int i = 0; i < _current_capacity; i++){
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

    
    for(int i = 0; i < __i; i++){
        int current_row = 0;
        int c;
        for(c = 0; tokens[i].string_[c] != '\0'; c++){
            //unsigned char* x_jump = &(rows[current_row].items[tokens[i].string_[c]].x_jump);
            //printf("%i\n",current_row);
            if(rows[current_row].items[tokens[i].string_[c]].a == 0){
                rows[current_row].items[tokens[i].string_[c]].a = height;
                if(height > current_capacity){
                    //non of this works
                    current_capacity++;
                    int size = current_capacity*tree_row_size;
                    tree_row nrows[size];
                    int length = sizeof(rows)/sizeof(rows[0]);    
                    for (int _i = 0; _i < length; _i++) {     
                        nrows[i] = rows[i];
                    }      
                    //free(rows);
                    //rows = nrows;
                }
                height++;
            }
            current_row = rows[current_row].items[tokens[i].string_[c]].a;
            
        }
        c--;
        rows[current_row].items[tokens[i].string_[c]].x_jump = tokens[i].outp;
        
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
