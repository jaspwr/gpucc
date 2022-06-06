// This needs a lot of redesigning to work with in conjuection with Yacc

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
using std::string;
#include <iostream>
using std::cout;

int __i = 0;
struct token{
    int outp;
    string string_;
};
const int len = 256;
token tokens[len];
int add_token(const char* str){
    tokens[__i].outp = __i+1;
    //printf("%s\n", str);
    tokens[__i].string_ = (string)str;
    //std::cout << tokens[__i].string_;
    __i++;
    return __i;
}



struct tree_row_item{
    unsigned char x_jump = 0;   
    unsigned char exit_id = 0;
    unsigned char b = 255;
    unsigned char a = 0;
};

struct tree_row{
    int depth;
    char leading_char;
    tree_row_item items[256];
};

struct token_tree
{
    unsigned char* data;
    int height;
};

int current_capacity = 256;
const int _current_capacity = 256;
tree_row *rows;

tree_row* scuffed_fix;
void flush_tree(){
    __i = 0;
    tree_row* rows1 = rows;
    tree_row t[_current_capacity];
    rows = t;
    if(sizeof(&rows) > 32)
        scuffed_fix = rows1;
        //delete &rows1;
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
    int __i;
    for(__i = 0; str[__i] != '\0'; __i++){
        //printf("\n%i\n\n",rows[row].items[str[__i]].a);
        row = rows[row].items[str[__i]].a;
        
    }
    __i--;
    //printf("\n%i\n\n",rows[row].items[str[__i]].x_jump);


    token_tree ret;
    ret.data = (unsigned char*)rows;
    ret.height = height;
    //free(rows);
    return ret;
}
