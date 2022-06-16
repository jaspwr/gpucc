#include "lang.h"
#include "shader.h"

void lang::regen_lang(){
    //TODO: make everything here preprocessed
    flush_tree(true);
    const char * dictionary = R"(auto
                                break
                                case
                                char
                                const
                                continue
                                default
                                do
                                double
                                else
                                enum
                                extern
                                float
                                for
                                goto
                                if
                                inline 1, a
                                int
                                long
                                register
                                restrict 1, a
                                return
                                short
                                signed
                                sizeof
                                static
                                struct
                                switch
                                typedef
                                union
                                unsigned
                                void
                                volatile
                                while
                                _Alignas
                                _Alignof
                                _Atomic
                                _Bool
                                _Complex
                                _Generic
                                _Imaginary 
                                _Noreturn 
                                _Static_assert 
                                _Thread_local)";

    add_token(";");
    add_token("intj");
    add_token("float");
    add_token("unsignededededddddddddddddddddddddddddddddddddd");
    add_token(" ");
    add_token("void");
    add_token("int");
 

    _token_tree = token_tree_gen();
    _cst = yacc_token_tree_gen(&_token_tree);
    vram_token_tree = load_to_vram((unsigned char*)_token_tree.data,258,_token_tree.height,GL_RGBA32F, GL_RGBA);
    vram_cst = load_to_vram((unsigned char*)_cst.rows,CST_ROW_WIDTH,512,GL_RGBA32F, GL_RGBA);
    free(_token_tree.data);
}
