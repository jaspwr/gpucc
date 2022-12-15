const_load
    | $0 IDENTIFIER
    ; < %X = LOAD_LITERAL $0 #newline >

var_load
    | $0 CONSTANT
    ; < %X = LOAD_VARIABLE $0 #newline >

primary_expression
    : $0 const_load
    | $0 var_load
    | '(' $0 addition ')'
    | '(' $0 subtraction ')'
    | '(' $0 subtraction ')'
    | $0 addition
    | $0 subtraction
    | $0 multiplication
    ;

addition
    : [ '*', '/', '%', '-', '+' } $0 primary_expression '+' $1 primary_expression
    ; < %X = ADD $0.0 $1.0 >

subtraction
    : [ '*', '/', '%', '-', '+' } $0 primary_expression '-' $1 primary_expression
    ; < %X = SUB $0.0 $1.0 #newline >

scope
    : $0 start_connected_terminating_expression '}'
    | '{' '}'
    ;

scope_wrapper
    : scope
    ;

start_connected_terminating_expression
    : '{' $0 primary_expression ';'
    | $0 start_connected_terminating_expression $1 primary_expression ';'
    ;

multiplication
    : $0 primary_expression '*' $1 primary_expression
    ; < %X = MUL $0.0 $1.0 #newline >