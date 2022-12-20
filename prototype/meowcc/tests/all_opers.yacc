const_load
    | $0 IDENTIFIER
    ;

var_load
    | [ '=' } $0 CONSTANT
    ; < %X = LOAD_VARIABLE $0 #newline >

equal
    | $0 primary_expression '='
    ;

cast_expression
    | $0 primary_expression $1 primary_expression
    ; < %X = CAST $0 $1 #newline >

primary_expression
    : { primary_expression } $0 const_load
    | { primary_expression } $0 var_load
    | { primary_expression } '(' $0 primary_expression ')'
    | { primary_expression } $0 addition
    | { primary_expression } $0 subtraction
    | { primary_expression } $0 multiplication
    | { primary_expression } $0 division
    | { primary_expression } $0 modulo
    | { primary_expression } $0 cast_expression
    | { primary_expression } $0 unary_expression
    ;

addition
    : [ '*', '/', '%', '-' } $0 primary_expression '+' $1 primary_expression
    ; < %X = ADD $0 $1 #newline>

subtraction
    : [ '*', '/', '%' } $0 primary_expression '-' $1 primary_expression
    ; < %X = SUB $0 $1 #newline >

start_connected_terminating_expression
    : '{' $0 primary_expression ';'
    | $0 start_connected_terminating_expression $1 primary_expression ';'
    | $0 start_connected_terminating_expression $1 start_connected_terminating_expression
    | $0 assignment
    ; < >

multiplication
    : [ '/', '%' } $0 primary_expression '*' $1 primary_expression
    ; < %X = MUL $0 $1 #newline >

division
    : [ '%' } $0 primary_expression '/' $1 primary_expression
    ; < %X = DIV $0 $1 #newline >

modulo
    : $0 primary_expression '%' $1 primary_expression
    ; < %X = MOD $0 $1 #newline >

assignment
    : $0 equal $1 primary_expression ';'
    ; < STORE_VARIABLE $0 $1 #newline >

unary_minus
    : { primary_expression, ')' } '-' $0 primary_expression
    ; < %X = SUB 0 $0 #newline >

unary_expression
    : $0 unary_minus
    | $0 unary_increment_pre
    | $0 unary_increment_post
    | $0 unary_decrement_pre
    | $0 unary_decrement_post
    ;

scope
    : $0 start_connected_terminating_expression '}'
    ;