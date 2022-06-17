#include "lang.h"
#include "shader.h"

void lang::regen_lang(){
    //TODO: make everything here preprocessed
    flush_tree(true);
    const char * dictionary = R"((
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
?:	
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
_Complex
_Generic
_Imaginary 
_Noreturn 
_Static_assert 
_Thread_local)";

    #define MAX_LENGTH 10000
    int substr_len = 1;
    int token_start = 0;
    int token_i = 1;
    for(int i = 0; i < MAX_LENGTH && dictionary[i] != '\0'; i++){
        if(dictionary[i+1] <= ' '){
            //White space
            if(substr_len != 0){
                char substr[substr_len + 1];
                for(int ii = 0; ii < substr_len; ii++){
                    substr[ii] = dictionary[token_start + ii];
                }
                substr[substr_len] = '\0';
                //printf("%s___", substr);
                add_token(substr, token_i);
                token_i++;
            }
            substr_len = 0;
            token_start = i+2;
        }else{
            //Token
            substr_len++;
        }
    }

    #undef MAX_LENGTH
 

    _token_tree = token_tree_gen();
    _cst = gen_tree(&_token_tree,token_i);
    vram_token_tree = load_to_vram((unsigned char*)_token_tree.data,258,_token_tree.height,GL_RGBA32F, GL_RGBA);
    vram_cst = load_to_vram((unsigned char*)_cst.data,258,_cst.height,GL_RGBA32F, GL_RGBA);
    free(_token_tree.data);
    free(_cst.data);
}
