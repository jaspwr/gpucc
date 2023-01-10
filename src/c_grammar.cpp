const char* c_yacc = R"(

primary_expression
    : $0 { #identifier, type_specifier, type_qualifier } #identifier
    | $0 #literal
    | ] 'if', 'switch', 'for' } '(' $0 primary_expression ')'
    | $0 addition
    | $0 subtraction
    | $0 multiplication
    | $0 division
    | $0 modulo
    | $0 unary_minus
    | $0 unary_plus
    | $0 shift_right
    | $0 shift_left
    | $0 comparison_less_than
    | $0 comparison_greater_than
    | $0 comparison_less_than_or_equal
    | $0 comparison_greater_than_or_equal
    | $0 comparison_equal
    | $0 comparison_not_equal
    | $0 bitwise_and
    | $0 bitwise_or
    | $0 bitwise_xor
    | $0 unary_not
    | $0 unary_bitwise_not
    | $0 postfix_increment
    | $0 postfix_decrement
    | $0 prefix_increment_second
    | $0 prefix_decrement_second
    | $0 assign
    | $0 bool_and
    | $0 bool_or
    | $0 bool_not
    | $0 ternary_selection
    | $0 add_assign
    | $0 sub_assign
    | $0 mul_assign
    | $0 div_assign
    | $0 mod_assign
    | $0 shl_assign
    | $0 shr_assign
    | $0 and_assign
    | $0 xor_assign
    | $0 or_assign
    | $0 method_call_no_args
    | $0 method_call
    | $0 array_access_wrapper
    ;

method_call_head
    : $0 primary_expression '('
    ;

method_call_no_args
    : $0 method_call_head ')'
    ; < $x = CALL $0 ()
    >

method_call
    : $0 argument_list ')'
    | $0 argument_list_open ')'
    ; < replace_with $x 
    !0 )
    >

array_access
    : $0 primary_expression '[' $1 primary_expression ']'
    ; < $x = GETELEMENTPTR $0 $1
    >

array_access_wrapper
    : $0 array_access
    ; < $x = LOAD $0
    >

argument_list_open
    : [ '(', '[', '.', '->' } [ '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '^=', '++', '--', '&&', '||', '?', ':' }
    $0 method_call_head $1 primary_expression
    ; < replace_me = CALL $0 ( $1 >

argument_list
    : [ '(', '[', '.', '->' } [ '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '^=', '++', '--', '&&', '||', '?', ':' }
    $1 argument_list_open ',' $0 primary_expression
    | $1 argument_list ',' $0 primary_expression
    ; < , $0 >

comma
    : ] ',', method_call_head } [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '^=', '|=', '++', '--', '&&', '||', '?', ':' } 
     $0 primary_expression ',' $1 primary_expression
    ; < $x = COMMA $0 $1 
    >

type_specifier
    : $0 'int'
    | $0 'float'
    | $0 'void'
    | $0 'char'
    | $0 'double'
    | $0 'long'
    | $0 struct_reference
    ;

struct_reference
    : 'struct' $0 #identifier
    ;

type_qualifier
    : $0 'const'
    | $0 'volatile'
    | $0 'unsigned'
    | $0 'signed'
    | $0 'short'
    ;

qualifier_list
    : $1 qualifier_list $0 type_qualifier
    | ] type_qualifier, qualifier_list } $0 type_qualifier
    ; < $0 >

declaration
    : ] type_qualifier, qualifier_list } $0 type_specifier $1 #identifier
    | ] type_qualifier, qualifier_list } $2 qualifier_list $0 type_specifier $1 #identifier
    ; < $x $0 $1 
    >

addition
    : [ '(', '[', '.', '->' } { '*', '/', '-', '%', '++', '--' } ] '+' } $0 primary_expression '+' $1 primary_expression
    ; < $x = ADD $0 $1 
    >

subtraction
    : [ '(', '[', '.', '->' } { '*', '/', '%', '++', '--' } ] '-' } $0 primary_expression '-' $1 primary_expression
    ; < $x = SUB $0 $1 
    >

multiplication
    : [ '(', '[', '.', '->' } { '/', '%', '++', '--' } ] '*' } $0 primary_expression '*' $1 primary_expression
    ; < $x = MUL $0 $1 
    >

division
    : [ '(', '[', '.', '->' } [ '++', '--' } ] '/' } $0 primary_expression '/' $1 primary_expression
    ; < $x = DIV $0 $1 
    >

modulo
    : [ '(', '[', '.', '->' } ] '%', '++', '--' } $0 primary_expression '%' $1 primary_expression
    ; < $x = MOD $0 $1 
    >

shift_right
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '++', '--' } ] '>>' } $0 primary_expression '>>' $1 primary_expression
    ; < $x = SHR $0 $1 
    >

shift_left
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '++', '--' } ] '<<' } $0 primary_expression '<<' $1 primary_expression
    ; < $x = SHL $0 $1 
    >

comparison_less_than
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', '++', '--' } ] '<' } $0 primary_expression '<' $1 primary_expression
    ; < $x = CMP LT $0 $1 
    >

comparison_greater_than
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '++', '--' } ] '>' } $0 primary_expression '>' $1 primary_expression
    ; < $x = CMP GT $0 $1 
    >

comparison_less_than_or_equal
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '>=', '++', '--' } ] '<=' } $0 primary_expression '<=' $1 primary_expression
    ; < $x = CMP LTE $0 $1 
    >

comparison_greater_than_or_equal
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '++', '--' } ] '>=' } $0 primary_expression '>=' $1 primary_expression
    ; < $x = CMP GTE $0 $1 
    >

comparison_equal
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '!=', '++', '--' } ] '==' } $0 primary_expression '==' $1 primary_expression
    ; < $x = CMP EQ $0 $1 
    >

comparison_not_equal
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '++', '--' } ] '!=' } $0 primary_expression '!=' $1 primary_expression
    ; < $x = CMP NE $0 $1 
    >

bitwise_and
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '!=', '==', '++', '--' } ] '&' } $0 primary_expression '&' $1 primary_expression
    ; < $x = AND $0 $1 
    >

bitwise_xor
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '!=', '==', '&', '++', '--' } ] '^' } $0 primary_expression '^' $1 primary_expression
    ; < $x = XOR $0 $1
    >

bitwise_or
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '!=', '==', '&', '^', '++', '--' } ] '|' } $0 primary_expression '|' $1 primary_expression
    ; < $x = OR $0 $1 
    >

unary_plus
    : [ '(', '[', '.', '->' } ] primary_expression , ')', addition, subtraction, multiplication, method_call_no_args, method_call,
        division, modulo, shift_right, shift_left, comparison_greater_than, array_access, array_access_wrapper,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, 
        postfix_increment, prefix_decrement, postfix_decrement, prefix_increment_second, prefix_decrement_second bool_and, bool_or, ternary_selection }
        '+' $0 primary_expression
    ;

unary_minus
    : [ '(', '[', '.', '->' } ] primary_expression , ')', addition, subtraction, multiplication, method_call_no_args, method_call,
        division, modulo, shift_right, shift_left, comparison_greater_than, array_access, array_access_wrapper,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, 
        postfix_increment, prefix_decrement, postfix_decrement, prefix_increment_second, prefix_decrement_second bool_and, bool_or, ternary_selection }
        '-' $0 primary_expression
    ; < $x = SUB 0 $0 
    >

unary_not
    : [ '(', '[', '.', '->' } ] primary_expression , ')', addition, subtraction, multiplication, method_call_no_args, method_call,
        division, modulo, shift_right, shift_left, comparison_greater_than, array_access, array_access_wrapper,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, 
        postfix_increment, prefix_decrement, postfix_decrement, prefix_increment_second, prefix_decrement_second bool_and, bool_or, ternary_selection }
        '!' $0 primary_expression
    ; < $x = BOOL_NOT $0 
    >

unary_bitwise_not
    : [ '(', '[', '.', '->' } ] primary_expression , ')', addition, subtraction, multiplication, method_call_no_args, method_call,
        division, modulo, shift_right, shift_left, comparison_greater_than, array_access, array_access_wrapper,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, 
        postfix_increment, prefix_decrement, postfix_decrement, prefix_increment_second, prefix_decrement_second bool_and, bool_or, ternary_selection }
        '~' $0 primary_expression
    ; < $x = XOR $0 -1
    >

prefix_increment
    : [ '(', '[', '.', '->' } '++' $0 primary_expression
    ; < $x = ADD $0 1
    STORE $0 $x
    >

prefix_increment_second
    : [ '(', '[', '.', '->' } $0 prefix_increment
    ; < $x = SUB $0 1 
    >

postfix_increment
    : [ '(', '[', '.', '->' } $0 primary_expression '++'
    ; < $x = ADD $0 1 
    STORE $0 $x 
    >

prefix_decrement
    : [ '(', '[', '.', '->' } '--' $0 primary_expression
    ; < $x = SUB $0 1 
    STORE $0 $x 
    >

prefix_decrement_second
    : [ '(', '[', '.', '->' } $0 prefix_decrement
    ; < $x = ADD $0 1 
    >

postfix_decrement
    : [ '(', '[', '.', '->' } $0 primary_expression '--'
    ; < $x = SUB $0 1
    STORE $0 $x
    >

bool_and
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '++', '--' } ] '&&' }
    $0 primary_expression '&&' $1 primary_expression
    ; < $x = BOOL_AND $0 $1 
    >

bool_or
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '++', '--', '&&' } ] '||' }
    $0 primary_expression '||' $1 primary_expression
    ; < $x = BOOL_OR $0 $1 
    >

ternary_selection
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '++', '--', '&&', '||' } ] '?' }
    $0 primary_expression '?' $1 primary_expression ':' $2 primary_expression
    ; < $x = SELECT $0 $1 $2 
    >

assign
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '++', '--', '&&', '||', '?', ':' } ] '=' }
    $0 primary_expression '=' $1 primary_expression
    ; < $x = $1
    STORE $0 $x 
    >

add_assign
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '++', '--', '&&', '||', '?', ':' } ] '+=' } 
    $0 primary_expression '+=' $1 primary_expression
    ; < $x = ADD $0 $1 
    STORE $0 $x 
    >

sub_assign
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '++', '--', '&&', '||', '?', ':' } ] '-=' } 
    $0 primary_expression '-=' $1 primary_expression
    ; < $x = SUB $0 $1 
    STORE $0 $x 
    >

mul_assign
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '++', '--', '&&', '||', '?', ':' } ] '*=' } 
    $0 primary_expression '*=' $1 primary_expression
    ; < $x = MUL $0 $1 
    STORE $0 $x 
    >

div_assign
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '++', '--', '&&', '||', '?', ':' } ] '/=' } 
    $0 primary_expression '/=' $1 primary_expression
    ; < $x = DIV $0 $1 
    STORE $0 $x 
    >

mod_assign
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '++', '--', '&&', '||', '?', ':' } ] '%=' } 
    $0 primary_expression '%=' $1 primary_expression
    ; < $x = MOD $0 $1 
    STORE $0 $x 
    >

shl_assign
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '++', '--', '&&', '||', '?', ':' } ] '<<=' } 
    $0 primary_expression '<<=' $1 primary_expression
    ; < $x = SHL $0 $1 
    STORE $0 $x 
    >

shr_assign 
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '++', '--', '&&', '||', '?', ':' } ] '>>=' } 
    $0 primary_expression '>>=' $1 primary_expression
    ; < $x = SHR $0 $1 
    STORE $0 $x 
    >

and_assign
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '++', '--', '&&', '||', '?', ':' } ] '&=' } 
    $0 primary_expression '&=' $1 primary_expression
    ; < $x = AND $0 $1 
    STORE $0 $x 
    >

xor_assign
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '++', '--', '&&', '||', '?', ':' } ] '^=' } 
    $0 primary_expression '^=' $1 primary_expression
    ; < $x = XOR $0 $1 
    STORE $0 $x 
    >

or_assign
    : [ '(', '[', '.', '->' } { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '^=', '++', '--', '&&', '||', '?', ':' } ] '|=' }
    $0 primary_expression '|=' $1 primary_expression
    ; < $x = OR $0 $1 
        STORE $0 $x 
    >

partial_scope
    : $0 '{' $1 primary_expression ';'
    | $0 '{' $1 statement
    | $0 '{' $1 scope
    | $0 partial_scope $1 primary_expression ';'
    | $0 partial_scope $1 statement
    | $0 partial_scope $1 scope
    ;

scope
    : $0 partial_scope '}'
    ; < scope_start
    !0 scope_end
    >

if_head
    : 'if' '(' $0 primary_expression ')'
    ;

if
    : [ 'else', else } $0 if_head $1 primary_expression ';'
    | [ 'else', else } $0 if_head $1 scope
    ; < JZ $0 $x
        !1 $x :
    >

else
    : 'else' $0 primary_expression ';'
    | 'else' $0 scope
    ; < $x :
    >

if_else
    : $0 if_head $1 primary_expression ';' $2 else
    | $0 if_head $1 scope $2 else
    ; < JZ $0 $x
        !1 JMP $2
        $x :
        !2 >

switch_head
    : 'switch' '(' $0 primary_expression ')'
    ;

switch
    : $0 switch_head $1 switch_body
    ; < switch $0 switch_body_start
        !1 switch_body_end
        breakable $x :
    >

switch_case_opener
    : 'case' $0 primary_expression ':'
    ; < switch_case $0
    >

default_opener
    : 'default' ':'
    ; < switch_default
    >

switch_case
    : $0 switch_case_opener $1 primary_expression ';'
    | $0 switch_case_opener $1 statement
    | $0 switch_case_opener $1 scope
    | $0 default_opener $1 primary_expression ';'
    | $0 default_opener $1 statement
    | $0 default_opener $1 scope
    | $0 switch_case $1 primary_expression ';'
    | $0 switch_case $1 statement
    | $0 switch_case $1 scope
    ;

parial_switch_body
    : $0 switch_case '}'
    | $0 switch_case $1 parial_switch_body
    ;

switch_body
    : '{' $0 parial_switch_body
    ;

while_head
    : 'while'
    ; < $x:
    >

while
    : $0 while_head $1 primary_expression $2 primary_expression ';'
    | $0 while_head $1 primary_expression $2 scope
    ; < !0 !1 JZ $1 $x 
        !2 JMP $0
        breakable $x :
    >

for_head
    : 'for' '(' $0 primary_expression ';'
    ; < $x:
    >

for
    : $0 for_head $1 primary_expression ';' $3 primary_expression ')' $2 primary_expression ';'
    | $0 for_head $1 primary_expression ';' $3 primary_expression ')' $2 statement
    | $0 for_head $1 primary_expression ';' $3 primary_expression ')' $2 scope
    ; < JZ $1 $x
        !2 !3 JMP $0
        breakable $x : 
    >

break
    : 'break' ';'
    ; < BREAK
    >

statement
    : $0 if
    | $0 if_else
    | $0 while
    | $0 break
    | $0 switch
    | $0 for
    ;
)";