const_load
    : $0 IDENTIFIER
    ;

var_load
    : $0 CONSTANT
    ; < %X = LOAD_VARIABLE $0 #newline >

equal
    : $0 primary_expression '=' $1 primary_expression ';'
    ; < STORE_VARIABLE $0 $1 #newline >

plus_equal
    : $0 primary_expression '+=' $1 primary_expression ';'
    ; < %X = ADD $0 $1 #newline>

minus_equal
    : $0 primary_expression '-=' $1 primary_expression ';'
    ; < %X = SUB $0 $1 #newline>

unary_increment_post
    : $0 primary_expression '++'
    ; < %X = ADD $0 1 #newline>

unary_decrement_post
    : $0 primary_expression '--'
    ; < %X = SUB $0 1 #newline>

relative_assignment_wrapper
    : $0 plus_equal
    | $0 minus_equal
    ; < STORE_VARIABLE $0.0 $0 #newline >

inc_dec_wrapper
    : $0 unary_increment_post
    | $0 unary_decrement_post
    ; < STORE_VARIABLE $0.0 $0 #newline >
    

primary_expression
    : { primary_expression } $0 const_load
    | { primary_expression } $0 var_load
    | { primary_expression } '(' $0 primary_expression ')'
    | { primary_expression } $0 addition
    | { primary_expression } $0 subtraction
    | { primary_expression } $0 multiplication
    | { primary_expression } $0 division
    | { primary_expression } $0 modulo
    | { primary_expression } $0 unary_minus
    | { primary_expression } $0 inc_dec_wrapper
    | { primary_expression } $0 function_call
    ;

function_call
    : $0 primary_expression '(' $1 primary_expression ')'
    | $0 primary_expression '(' ')'
    ; < %X = CALL $0.0 ( $1 ) #newline >

addition
    : [ '*', '/', '%', '-', '++', '--' } $0 primary_expression '+' $1 primary_expression
    ; < %X = ADD $0 $1 #newline>

subtraction
    : [ '*', '/', '%', '++', '--' } $0 primary_expression '-' $1 primary_expression
    ; < %X = SUB $0 $1 #newline >

start_connected_terminating_expression
    : '{' $0 primary_expression ';'
    | $0 start_connected_terminating_expression $1 primary_expression ';'
    | $0 start_connected_terminating_expression $1 start_connected_terminating_expression
    | $0 equal
    | $0 relative_assignment_wrapper
    ; < >

multiplication
    : [ '/', '%', '++', '--' } $0 primary_expression '*' $1 primary_expression
    ; < %X = MUL $0 $1 #newline >

division
    : [ '%', '++', '--' } $0 primary_expression '/' $1 primary_expression
    ; < %X = DIV $0 $1 #newline >

modulo
    : $0 primary_expression '%' $1 primary_expression
    ; < %X = MOD $0 $1 #newline >


unary_minus
    : { primary_expression, ')' } '-' $0 primary_expression
    ; < %X = SUB 0 $0 #newline >
    

scope
    : $0 start_connected_terminating_expression '}'
    | '{' '}'
    ; < Xl #newline>