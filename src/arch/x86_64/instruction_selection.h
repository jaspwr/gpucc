const char* inst_selection_schema = R"(
%a:i32 = ADD %b:i32 %c:i32 ->
    mov %a %b
    add %a %c
    ;

%a:i32 = MUL %b:i32 %c:i32 ->
    mov %a %b
    imul %a %c
    ;

JMP %a:_ ->
    jmp %a
    ;

%a:_ = COPY %b:_ ->
    mov %a %b
    ;

%a:i1 = CMP EQ %b:i32 %c:i32
JZ %a:i1 %d:_ ->
    cmp %b %c
    jne %d
    ;
)";

