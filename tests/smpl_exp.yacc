const_load
    | $0 IDENTIFIER
    ; < %X = LOAD_LITERAL $0 #newline >

primary_expression
    : $0 const_load
    | $0 CONSTANT
    | '(' $0 primary_expression ')'
    | $0 addition
    | $0 subtraction
    ; < %X = passthroughnode $0 #newline >

addition
    : [ '*', '/', '%', '-', '+' } $0 primary_expression '+' $1 primary_expression
    ; < %X = ADD $0 $1 #newline >

subtraction
    : [ '*', '/', '%', '-', '+' } $0 primary_expression '-' $1 primary_expression
    ; < %X = SUB $0 $1 #newline >

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