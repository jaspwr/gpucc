var_load
    : $0 CONSTANT
    ; < %X = LOAD $0 #newline >

primary_expression
    : $0 IDENTIFIER
    | $0 var_load
    | '(' $0 primary_expression ')'
    | $0 addition
    | $0 subtraction
    ;

addition
    : $0 primary_expression '+' $1 primary_expression
    ; < %X = ADD $0.0 $1.0 #newline >

subtraction
    : $0 primary_expression '-' $1 primary_expression
    ; < %X = SUB $0.0 $1.0 #newline >

scope
    : $0 start_connected_terminating_expression '}'
    | '{' '}'
    ; < Xl #newline>

start_connected_terminating_expression
    : '{' $0 primary_expression ';'
    | $0 start_connected_terminating_expression $1 primary_expression ';'
    | '{' $0 assignment
    | start_connected_terminating_expression $0 assignment
    | '{' $0 full_branch
    | start_connected_terminating_expression $0 full_branch
    ;