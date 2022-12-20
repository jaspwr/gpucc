STRING_LITERAL
    : '"' ... '"'
    ;

CONSTANT
    : CONSTANT '.' CONSTANT
	;

variable_name_read
    : $0 IDENTIFIER
    ; < %X = load #newline >

variable_name_write
    : $0 IDENTIFIER assignment_operator
    ; < %X = load ptr #newline >


array_call
    : $1 primary_expression '[' $0 primary_expression ']'
    ; < %X = getelementptr inbounds IMPLICITTYPE , IMPLICITTYPE * $1, i64 0, i64 $0 #newline >

array_call_loaded
    : $0 array_call
    ; < %X = load IMPLICITTYPE , IMPLICITTYPE * $0 #newline >

array_call_write
    : $1 primary_expression '[' $0 primary_expression ']' assignment_operator
    ; < %X = getelementptr inbounds IMPLICITTYPE , IMPLICITTYPE * $1, i64 0, i64 $0 #newline >

primary_expression
    : $0 CONSTANT
    | $0 variable_name_read
	| $0 STRING_LITERAL
    | $0 unary_operation
    | '(' $0 primary_expression ')'
    | $0 expression
    | $0 unary_bool_not
    | $0 unary_bool_negation
    | $0 unary_minus
    | $0 unary_increment
    | $0 unary_decrement
    | $0 ternary_conditional_expression
    | $0 mutliplication
    | $0 division
    | $0 modulo
    | $0 addition
    | $0 subtraction
    | $0 bitshift_left
    | $0 bitshift_right
    | $0 less_than
    | $0 less_than_or_equal_to
    | $0 greater_than
    | $0 greater_than_or_equal_to
    | $0 equality
    | $0 inequality
    | $0 logical_and
    | $0 logical_or
    | $0 logical_xor
    | $0 logical_not
    | $0 bitwise_and
    | $0 bitwise_or
    | $0 bitwise_xor
    | $0 function_whole
    | $0 function_whole_no_args
    | $0 array_call_loaded
	;


branch
    : 'if' $0 primary_expression 
    ; <brandc #newline>

full_branch
    : $0 branch $1 scope
    | $0 branch $1 scope 'else' $2 scope
    ; < br i1 $0.0.0, label $1, label #newline>


function_name
    : $0 IDENTIFIER '('
    ; < ) #newline >

function_partial_terminating
    : ',' $0 primary_expression ')'
    | ',' $0 primary_expression $1 function_partial_terminating
    ; < , i32 noundef $0 >

function_whole
    : $1 function_name $0 primary_expression ')'
    | $2 function_name $0 primary_expression $1 function_partial_terminating
    ; < %X = call i32 @name( i32 noundef $0 >

function_whole_no_args
    : $1 function_name ')'
    ; < %X = call i32 @name( >


start_connected_terminating_expression
    : '{' $0 primary_expression ';'
    | $0 start_connected_terminating_expression $1 primary_expression ';'
    | '{' $0 assignment
    | start_connected_terminating_expression $0 assignment
    | '{' $0 full_branch
    | start_connected_terminating_expression $0 full_branch
    ;

scope
    : $0 start_connected_terminating_expression '}'
    | '{' '}'
    ; < Xl #newline>


assignment
    : $1 variable_name_write $0 primary_expression ';'
    | $1 array_call_write $0 primary_expression ';'
    ; <store i32 $0.0, i32 * $1 #newline>

unary_bool_not
    :  { ')', primary_expression} '!' $0 primary_expression
    ; < %X = xor i1 $0.0, true #newline >

unary_bitwise_not
    :  { ')', primary_expression} '~' $0 primary_expression
    ; < %X = xor i32 $0.0, -1 #newline >

unary_minus
    :  { ')', primary_expression} '-' $0 primary_expression
    ; < %X = sub nsw i32 0, $0.0 #newline >

unary_increment
    : $0 primary_expression '++'
    ; < %X = add nsw i32 $0.0, 1 #newline >

unary_decrement
    : $0 primary_expression '--'
    ; < %X = sub nsw i32 $0.0, 1 #newline >



addition
    : [ '*', '/', '%'} $0 primary_expression '+' $1 primary_expression
    ; < %X = add nsw i32 $0.0, $1.0 #newline >

subtraction
    : [ '*', '/', '%' } $0 primary_expression '-' $1 primary_expression
    ; < %X = sub nsw i32 $0.0, $1.0 #newline >

mutliplication
    : $0 primary_expression '*' $1 primary_expression
    ; < %X = mul nsw i32 $0.0, $1.0 #newline >

division
    : $0 primary_expression '/' $1 primary_expression
    ; < %X = sdiv i32 $0.0, $1.0 #newline >

modulo
    : $0 primary_expression '%' $1 primary_expression
    ; < %X = srem i32 $0.0, $1.0 #newline >



logical_greater_than
    : [ '*', '/', '%', '-', '+', '<<', '>>'} $0 primary_expression '>' $1 primary_expression
    ; < %X = icmp sgt i32 $0.0, $1.0 #newline >

logical_less_than
    : [ '*', '/', '%', '-', '+', '<<', '>>'} $0 primary_expression '<' $1 primary_expression
    ; < %X = icmp slt i32 $0.0, $1.0 #newline >

logical_greater_than_or_equal
    : [ '*', '/', '%', '-', '+', '<<', '>>'} $0 primary_expression '>=' $1 primary_expression
    ; < %X = icmp sge i32 $0.0, $1.0 #newline >

logical_less_than_or_equal
    : [ '*', '/', '%', '-', '+', '<<', '>>' } $0 primary_expression '<=' $1 primary_expression
    ; < %X = icmp sle i32 $0.0, $1 #newline >

logical_comparison_expression
    : $0 logical_greater_than
    | $0 logical_less_than
    | $0 logical_greater_than_or_equal
    | $0 logical_less_than_or_equal
    ; < %X = zext i1 $0 to i32 #newline >


ternary_condition
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=', '==', '!=', '&', '^', '|', '&&', '||' } $0 primary_expression '?'
    ; < %X = icmp ne i32 $0.0, 0 #newline >

ternary_conditional_expression
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=', '==', '!=', '&', '^', '|', '&&', '||', '?' } $0 ternary_condition $1 primary_expression ':' $2 primary_expression
    ; <%X = select i1 $0, $1.0, $2.0 #newline >



logical_equality
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=' } $0 primary_expression '==' $1 primary_expression
    ; < %X = icmp eq i32 $0.0, $1.0 #newline >

logical_inequality
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=' } $0 primary_expression '!=' $1 primary_expression
    ; < %X = icmp ne i32 $0.0, $1.0 #newline >

logical_equality_expression
    : $0 logical_equality
    | $0 logical_inequality
    ; < %X = zext i1 $0 to i32 #newline >


logical_and
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=', '==', '!=', '&', '^', '|' } $0 primary_expression '&&' $1 primary_expression
    ; < %X = gwaaaaaaaa #newline >

logical_or
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=', '==', '!=', '&', '^', '|', '&&'} $0 primary_expression '||' $1 primary_expression
    ; < %X = meowmoewmoew #newline >


bitshift_left
    : [ '*', '/', '%', '-', '+'} $0 primary_expression '<<' $1 primary_expression
    ; < %X = shl i32 $0.0, $1.0 #newline >

bitshift_right
    : [ '*', '/', '%', '-', '+' } $0 primary_expression '>>' $1 primary_expression
    ; < %X = shr i32 $0.0, $1.0 #newline >

bitwise_xor
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=', '==', '!=', '&' } $0 primary_expression '^' $1 primary_expression
    ; < %X = xor i32 $0.0, $1.0 #newline >

bitwise_and
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=', '==', '!=' } $0 primary_expression '&' $1 primary_expression
    ; < %X = and i32 $0.0, $1.0 #newline >

bitwise_or
    : [ '*', '/', '%', '-', '+', '<<', '>>', '>', '>=', '<', '<=', '==', '!=', '&', '^' } $0 primary_expression '|' $1 primary_expression
    ; < %X = or i32 $0.0, $1.0 #newline >

assignment_operator
	: $0 '='
	| $0 '*='
	| $0 '/='
	| $0 '%='
	| $0 '+='
	| $0 '-='
	| $0 '<<='
	| $0 '>>='
	| $0 '&='
	| $0 '^='
	| $0 '|='
	;

ret
    : 'return' $0 primary_expression ';'
    ; < ret i32 $0.0.0 #newline >