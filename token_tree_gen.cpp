#include <stdio.h>
#include <stdlib.h>


int i = 0;
struct token{
    int outp;
    const char* string_;
};
const int len = 20;
token tokens[len];
void add_token(const char* str){
    tokens[i].outp = i+1;
    tokens[i].string_ = str;
    i++;
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



token_tree token_tree_gen(){
    add_token("int");
    add_token("intj");
    add_token("float");
    add_token("unsigned");
    add_token(" ");
    add_token("void");
    add_token(";");
    add_token("+");
    add_token("*");
    add_token("-");
    
    add_token("/");
    
    add_token("include");
    add_token("++");
    add_token("--");
    add_token("=");
    add_token("==");
    add_token("+=");
    add_token("-=");
    add_token("*=");
    add_token("/=");
    add_token("if");
    add_token("else");
    //add_token("switch");


    const int width = 256;
    int height = 1;
    int current_capacity = 256;
    int tree_row_size = sizeof(tree_row);
    //tree_row *rows = (tree_row*) malloc(tree_row_size*current_capacity);

    tree_row rows[current_capacity];
    
    for(int i = 0; i < len; i++){
        int current_row = 0;
        int c;
        for(c = 0; tokens[i].string_[c] != '\0'; c++){
            //unsigned char* x_jump = &(rows[current_row].items[tokens[i].string_[c]].x_jump);
            //printf("%i\n",current_row);
            if(rows[current_row].items[tokens[i].string_[c]].a == 0){
                rows[current_row].items[tokens[i].string_[c]].a = height;
                if(height > current_capacity){
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
    unsigned char str[] = "intj";
    unsigned char row = 0;
    int __i;
    for(__i = 0; str[__i] != '\0'; __i++){
        printf("\n%i\n\n",rows[row].items[str[__i]].a);
        row = rows[row].items[str[__i]].a;
        
    }
    __i--;
    printf("\n%i\n\n",rows[row].items[str[__i]].x_jump);


    token_tree ret;
    ret.data = (unsigned char*)rows;
    ret.height = height;
    //free(rows);
    return ret;
}
