#include "lang.h"
#include "shader.h"
#include "gl.h"

void lang::regen_lang(){
    //TODO: make everything here preprocessed
    flush_tree(true);
    const char * dictionary = R"(UNKNOWN;
ANYTHING;
IDENTIFIER;
LITERAL;
(
)
[
]
.
->
++
--
+
-
!
~
&
*
/
%
<<
>>
<=>
<
<=
>
>=
==
!=	
^
|
&&
||
?
:	
=
+=
-=
*=
/=
%=
<<=
>>=
&=
^=
|=
,
"
{
}
;
auto
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
inline
int
long
register
restrict
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
_Complex)";
    /*_Generic
    _Imaginary
    _Noreturn
    _Static_assert
    _Thread_local*/

    #define MAX_LENGTH 10000
    int substr_len = 1;
    int token_start = 0;
    int token_i = 1;
    for(int i = 0; i < MAX_LENGTH && dictionary[i] != '\0'; i++){
        if(dictionary[i+1] <= ' '){
            //White space
            if(substr_len != 0){
                char* substr = (char*)malloc(substr_len + 1);
                for(int ii = 0; ii < substr_len; ii++){
                    substr[ii] = dictionary[token_start + ii];
                }
                substr[substr_len] = '\0';
                //printf("%i\n", token_i);
                add_token(substr, token_i,0);
                yacc_parser::tokens.token_list[token_i] = substr;

                token_i++;
                free(substr);
            }
            substr_len = 0;
            token_start = i+2;
        }else{
            //Token
            substr_len++;
        }
    }

    #undef MAX_LENGTH
 
    ir_codegen::SSBO = (int*)malloc(6000 * sizeof(int));
    for (int i = 0; i < 256; i++) {
        ir_codegen::SSBO[i] = 0;
    }
    _token_tree = token_tree_gen();
    try {
        _cst = gen_tree(&_token_tree, token_i);
    }
    catch(char * e){
        print(PRINT_ERROR, e);
        throw;
    }
    //for (int i = 0; i < 256; i++) {
    //    printf("%i, ", _token_tree.data[4].items[i].x_jump);
    //    printf("%i\n",_token_tree.data[4].items[i].a);
    //}
    vram_token_tree = load_to_vram((unsigned char*)_token_tree.data,258,_token_tree.height,GL_RGBA32F, GL_RGBA);
    vram_cst = load_to_vram((unsigned char*)_cst.data,258,_cst.height,GL_RGBA32F, GL_RGBA);
    vram_codegen = load_to_ssbo(ir_codegen::current_SSBO_length, sizeof(int), ir_codegen::SSBO);
    free(_token_tree.data);
    free(_cst.data);
    free(ir_codegen::SSBO);
}

void lang::load_lang(char* preset){
    auto start = std::chrono::high_resolution_clock::now();
    char file[] = "presets/";
    strcat_s(file, preset);
    FILE* in;
	fopen_s(&in,file,"rb");
    if(in==NULL)
    {
        char prnt[] = "Could not locate compiler preset \"";
        strcat_s(prnt, preset);
        strcat_s(prnt, "\"");
        print(PRINT_ERROR, prnt);
    }else{
        #define BUFFER_SIZE (1 * 1024 * 1024)
        #define ITERATIONS (10 * 1024)
        //unsigned char buffer[BUFFER_SIZE]; // 1 MiB buffer
        //int i, x;
        //for(i = 0; i < ITERATIONS; ++i)
        //{
        //    fread(buffer, BUFFER_SIZE, 1, in);
        //}
        //GLuint c = load_to_vram(buffer,30000,1,GL_RGBA32F, GL_RGBA);
        #undef BUFFER_SIZE 
        #undef ITERATIONS
    }
} 
