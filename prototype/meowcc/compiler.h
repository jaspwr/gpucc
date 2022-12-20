#pragma once
#include "inc.h"
#include "lang.h"

extern const char* source;

struct preproc_ret {
    GLuint ssbo;
    int len;
};

class compiler{
    public:
        enum _debugging_config {
            none,
            ir_printout,
            ast_nodes_printout,
            tokens_printout
        };
		
        // I have grown since writing this (learned Rust) and I plan to refector
        // so it's not using mutable statics.
        inline static bool comps = true;
        inline static bool regens_lang = true;
        inline static char preset[] = "c";
        inline static bool verbose_output = false;
        inline static _debugging_config debugging_config = none;
        inline static std::string* custom_yacc = nullptr;

        static preproc_ret preprocessor(const char* source_file_path);
        static int token_length(const char* source, int start_position);
        static int* compile(GLuint source_vram, int source_len);

    private:
        compiler() {}
};

class preprocessor_directive {
public:
	inline static int self_count;
    inline static std::list<preprocessor_directive*> self_list;

	int val;
	const char* string;
	preprocessor_directive(const char* str) {
		string = str;
        self_list.push_back(this);
		val = self_count++;
	}
};