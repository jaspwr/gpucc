const char* x86_64_selection = R"(
%a:_ = ADD %b:i32 %c:i32 ->
    mov %a, %b
    add %a, %c
    ;

%a:_ = MUL %b:i32 %c:i32 ->
    mov %a, %b
    imul %a, %c
    ;

%a:_ = MUL %b:i32 %c:i32_l ->
    mov %a, %b
    imul %a, DWORD PTR [ %c ]
    ;

JMP %a:_ ->
    jmp %a
    ;

%a:_ = COPY %b:_ ->
    mov %a, %b
    ;

%a:_ = CMP EQ %b:i32 %c:i32
JZ %a:_, %d:_ ->
    cmp %b, %c
    jne %d
    ;
)";

