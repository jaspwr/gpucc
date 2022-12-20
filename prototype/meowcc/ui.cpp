#include "ui.h"
#include "term_cols.h"
#include <string>

enum TermColor {
    // TODO: Support more, not needed for now
    Red,
    Yellow,
    Green,
    White,
};

struct ColouredString {
    TermColor col;
    const char* text;
};

const ColouredString indicators[]{
    { TermColor::Green, "S" },
    { TermColor::Yellow, "i" },
    { TermColor::Yellow, "w" },
    { TermColor::Red, "E" }
};

void print_term_col(TermColor col) {
    switch (col)
    {
    Red:
        printf(RED);
        break;
    Yellow:
        printf(YEL);
        break;
    Green:
        printf(GRN);
        break;
    White:
        printf(WHT);
        break;
    default:
        break;
    }
}

void print(int type, const char* message) {
    printf("[");
    print_term_col(indicators[type].col);
    printf("%s ", indicators[type].text);
    printf(reset);
    printf("] ");
    printf("%s\n",message);
}

void print(int type, int message) {
    printf("[");
    print_term_col(indicators[type].col);
    printf("%s ", indicators[type].text);
    printf(reset);
    printf("] ");
    printf("%d\n",message);
}

typedef bool skips_next;

skips_next parse_flag(char* arg, char* next_arg) {
    if(!strcmp(arg, "--dbg-mode")) {
        if(next_arg != nullptr) {
            compiler::debugging_config = (compiler::_debugging_config)parse_dbg_mode(next_arg);
            printf("Setting debug mode to %s\n", next_arg);
        }
        return true;
    } else if (!strcmp(arg, "--yacc")) {
        if (next_arg != nullptr) {
            compiler::custom_yacc = new std::string(next_arg);
            printf("Using custom grammar %s\n", next_arg);
        }
        return true;
    } else {
        // Unknown flag
        print(PRINT_ERROR, "Unrecognised flag.");
        throw;
    }
}

char* parse_args(int argc, char** argv){
    printf(" /\\_/\\\n( o.o )   ~~ meowcc verion α 0.1 ~~\n > C < \n");
    if(argc != 2) {
        if(argc == 1)
            print(PRINT_ERROR, "File not specified");
    }
    char* file = nullptr;
    compiler::regens_lang = true;
    for(int i = 1; i < argc; i++) {
        char* next_arg = nullptr;
        if(i + 1 < argc) {
            next_arg = argv[i + 1];
        }
        if(argv[i][0] == '-'){
            // Is flag
            if(parse_flag(argv[i], next_arg)) {
                // skips next
                if(i + 1 < argc) {
                    i++;
                } else {
                    print(PRINT_ERROR, "Flag expects an argument.");
                    throw;
                }
            }
        }
        else {
            // Not flag
            file = argv[i];
        }
    }
    if(file == nullptr) {
        print(PRINT_ERROR, "No file specified.");
        throw;
    }
    return file;
}

int parse_dbg_mode(char* str) {
    if (!strcmp(str, "ir")) {
         return compiler::_debugging_config::ir_printout;
    }
    else if (!strcmp(str, "ast")) {
        return compiler::_debugging_config::ast_nodes_printout;
    }
    else if (!strcmp(str, "tok")) {
        return compiler::_debugging_config::tokens_printout;
    }
    else {
        print(PRINT_ERROR, "Unrecognised debugging mode");
        throw;
    }
    return compiler::_debugging_config::none;
}