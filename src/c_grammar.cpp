const char* c_pre_yacc = R"(
primary_expression
    : $0 { #identifier, type_specifier, type_qualifier, struct_union_type
    'int', 'float', 'char', 'void', 'double' 'long' 'struct', 'union' } #identifier
    | $0 #literal
    | ] 'if', 'switch', 'for' } '(' $0 primary_expression ')'
    | $0 sizeof
    ;

switch_case_opener
    : 'case' $0 primary_expression ':'
    ; < switch_case $0 $x :
    >

default_opener
    : 'default' ':'
    ; < switch_default
    >

sizeof
    : 'sizeof' '(' $0 qualifier_list $1 type_specifier ')'
    | 'sizeof' '(' $1 type_specifier ')'
    | 'sizeof' '(' $0 qualifier_list $1 pointer ')'
    | 'sizeof' '(' $1 pointer ')'
    | 'sizeof' '(' $0 qualifier_list $1 struct_union_type ')'
    | 'sizeof' '(' $1 struct_union_type ')'
    ; < $x = sizeof !0 !1
    >

type_specifier
    : $0 int
    | $0 char
    | $0 float
    | $0 void
    ; < !0 >

char
    : 'char'
    ; < i8 >

int
    : 'int'
    ; < i32 >

float
    : 'float'
    ; < f32 >

void
    : 'void'
    ; < void >

pointer
    : $0 type_specifier '*'
    | $0 pointer '*'
    | $0 struct_union_type '*'
    ; < !0 ptr >

struct_union_type
    : $0 union_type
    | $0 struct_type
    ;

struct_type
    : 'struct' $1 #identifier
    ; < struct $1 >

union_type
    : 'union' $1 #identifier
    ; < union $1 >

type_qualifier
    : $0 'const'
    | $0 'volatile'
    | $0 'unsigned'
    | $0 'signed'
    | $0 'short'
    | $0 'register'
    ;

qualifier_list
    : $1 qualifier_list $0 type_qualifier
    | ] type_qualifier, qualifier_list } $0 type_qualifier
    ; < $0 >

cast_operator
    : ] 'sizeof' } '(' $0 qualifier_list $1 type_specifier ')'
    | ] 'sizeof' } '(' $1 type_specifier ')'
    | ] 'sizeof' } '(' $0 qualifier_list $1 pointer ')'
    | ] 'sizeof' } '(' $1 pointer ')'
    | ] 'sizeof' } '(' $0 qualifier_list $1 struct_union_type ')'
    | ] 'sizeof' } '(' $1 struct_union_type ')'
    ; < !0 !1 >

declaration
    : ] type_qualifier, qualifier_list, #identifier } $1 type_specifier $2 #identifier
    | ] type_qualifier, qualifier_list, #identifier } $1 type_specifier $2 primary_expression
    | ] type_qualifier, qualifier_list, #identifier } $1 struct_union_type $2 #identifier
    | ] type_qualifier, qualifier_list, #identifier } $1 struct_union_type $2 primary_expression
    | ] type_qualifier, qualifier_list, #identifier } $1 pointer $2 primary_expression
    | ] type_qualifier, qualifier_list, #identifier } $0 qualifier_list $1 type_specifier $2 #identifier
    | ] type_qualifier, qualifier_list, #identifier } $0 qualifier_list $1 struct_union_type $2 #identifier
    | ] type_qualifier, qualifier_list, #identifier } $0 qualifier_list $1 type_specifier $2 primary_expression
    | ] type_qualifier, qualifier_list, #identifier } $0 qualifier_list $1 struct_union_type $2 primary_expression
    | ] type_qualifier, qualifier_list, #identifier } $0 qualifier_list $1 pointer $2 primary_expression
    ; < !0 !1 $2 >

declaration_list
    : $0 declaration_list $1 declaration ','
    | ] ',', declaration_list } $0 declaration ','
    ; < , >

function_definition_head
    : $0 declaration '(' $1 declaration_list $2 declaration ')'
    | $0 declaration '(' $1 declaration ')'
    | $0 declaration '(' ')'
    ; < define !0 ( !1 !2 ) >

method_call_head
    : $0 primary_expression '('
    ;

)";

const char* c_yacc = R"(

primary_expression
    : $0 { #identifier, type_specifier, type_qualifier, 
    'int', 'float', 'char', 'void', 'double' 'long' 'struct' } #identifier
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
    | $0 declaration_full
    | $0 dereference
    | $0 address_of
    | $0 cast_expression
    ;

function_definition
    : $0 function_definition_head $1 scope
    ; < !0 { 
        !1}
    >

declaration_full
    : $0 declaration
    ; < $x = ALLOCA !0
    >

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
    ; < $x = GETELEMENTPTR $0 loadable $1
    >

array_access_wrapper
    : $0 array_access
    ; < $x = LOAD $0
    >

argument_list_open
    : [ '(', '['} { '.', '->' } [ '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '^=', '|=', '++', '--', '&&', '||', '?', ':' }
    $0 method_call_head $1 primary_expression
    ; < replace_me = CALL $0 ( loadable $1 >

argument_list
    : [ '(', '['} { '.', '->' } [ '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '^=', '|=', '++', '--', '&&', '||', '?', ':' }
    $1 argument_list_open ',' $0 primary_expression
    | $1 argument_list ',' $0 primary_expression
    ; < , loadable $0 >

comma
    : ] ',', method_call_head } [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '^=', '|=', '++', '--', '&&', '||', '?', ':' } 
     $0 primary_expression ',' $1 primary_expression
    ; < $x = COMMA loadable $0 loadable $1  
    >   

addition
    : [ '(', '['} { '.', '->' } { '*', '/', '-', '%', '++', '--' } ] '+' } $0 primary_expression '+' $1 primary_expression
    ; < $x = ADD loadable $0 loadable $1  
    >

subtraction
    : [ '(', '['} { '.', '->' } { '*', '/', '%', '++', '--' } ] '-' } $0 primary_expression '-' $1 primary_expression
    ; < $x = SUB loadable $0 loadable $1  
    >

multiplication
    : [ '(', '['} { '.', '->' } { '/', '%', '++', '--' } ] '*' } $0 primary_expression '*' $1 primary_expression
    ; < $x = MUL loadable $0 loadable $1  
    >

division
    : [ '(', '['} { '.', '->' } [ '++', '--' } ] '/' } $0 primary_expression '/' $1 primary_expression
    ; < $x = DIV loadable $0 loadable $1  
    >

modulo
    : [ '(', '['} { '.', '->' } ] '%', '++', '--' } $0 primary_expression '%' $1 primary_expression
    ; < $x = MOD loadable $0 loadable $1 
    >

shift_right
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '++', '--' } ] '>>' } $0 primary_expression '>>' $1 primary_expression
    ; < $x = SHR loadable $0 loadable $1  
    >

shift_left
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '++', '--' } ] '<<' } $0 primary_expression '<<' $1 primary_expression
    ; < $x = SHL loadable $0 loadable $1  
    >

comparison_less_than
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', '++', '--' } ] '<' } $0 primary_expression '<' $1 primary_expression
    ; < $x = CMP LT loadable $0 loadable $1  
    >

comparison_greater_than
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '++', '--' } ] '>' } $0 primary_expression '>' $1 primary_expression
    ; < $x = CMP GT loadable $0 loadable $1  
    >

comparison_less_than_or_equal
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '>=', '++', '--' } ] '<=' } $0 primary_expression '<=' $1 primary_expression
    ; < $x = CMP LTE loadable $0 loadable $1  
    >

comparison_greater_than_or_equal
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '++', '--' } ] '>=' } $0 primary_expression '>=' $1 primary_expression
    ; < $x = CMP GTE loadable $0 loadable $1 
    >

comparison_equal
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '!=', '++', '--' } ] '==' } $0 primary_expression '==' $1 primary_expression
    ; < $x = CMP EQ loadable $0 loadable $1 
    >

comparison_not_equal
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '++', '--' } ] '!=' } $0 primary_expression '!=' $1 primary_expression
    ; < $x = CMP NE loadable $0 loadable $1  
    >

bitwise_and
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '!=', '==', '++', '--' } ] '&' } $0 primary_expression '&' $1 primary_expression
    ; < $x = AND loadable $0 loadable $1 
    >

bitwise_xor
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '!=', '==', '&', '++', '--' } ] '^' } $0 primary_expression '^' $1 primary_expression
    ; < $x = XOR loadable $0 loadable $1
    >

bitwise_or
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '!=', '==', '&', '^', '++', '--' } ] '|' } $0 primary_expression '|' $1 primary_expression
    ; < $x = OR loadable $0 loadable $1 
    >

cast_expression
    : [ '(', '['} { '.', '->' } [ '++', '--' }
    $1 cast_operator $0 primary_expression
    ; < $x = CAST !1 loadable $0
    >

unary_plus
    : [ '(', '['} { '.', '->' } ] primary_expression , ')', addition, subtraction, multiplication, method_call_no_args, method_call,
        division, modulo, shift_right, shift_left, comparison_greater_than, array_access, array_access_wrapper,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, dereference, address_of,
        postfix_increment, prefix_decrement, postfix_decrement, prefix_increment_second, prefix_decrement_second bool_and, bool_or, ternary_selection }
        '+' $0 primary_expression
    ;

unary_minus
    : [ '(', '['} { '.', '->' } ] primary_expression , ')', addition, subtraction, multiplication, method_call_no_args, method_call,
        division, modulo, shift_right, shift_left, comparison_greater_than, array_access, array_access_wrapper,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, dereference, address_of,
        postfix_increment, prefix_decrement, postfix_decrement, prefix_increment_second, prefix_decrement_second bool_and, bool_or, ternary_selection }
        '-' $0 primary_expression
    ; < $x = SUB 0 loadable $0 
    >

unary_not
    : [ '(', '['} { '.', '->' } ] primary_expression , ')', addition, subtraction, multiplication, method_call_no_args, method_call,
        division, modulo, shift_right, shift_left, comparison_greater_than, array_access, array_access_wrapper,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, dereference, address_of,
        postfix_increment, prefix_decrement, postfix_decrement, prefix_increment_second, prefix_decrement_second bool_and, bool_or, ternary_selection }
        '!' $0 primary_expression
    ; < $x = XOR loadable $0 1 
    >

unary_bitwise_not
    : [ '(', '['} { '.', '->' } ] primary_expression , ')', addition, subtraction, multiplication, method_call_no_args, method_call,
        division, modulo, shift_right, shift_left, comparison_greater_than, array_access, array_access_wrapper,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, dereference, address_of,
        postfix_increment, prefix_decrement, postfix_decrement, prefix_increment_second, prefix_decrement_second bool_and, bool_or, ternary_selection }
        '~' $0 primary_expression
    ; < $x = XOR loadable $0 -1
    >

dereference
    : [ '(', '['} { '.', '->' } ] primary_expression , ')', addition, subtraction, multiplication, method_call_no_args, method_call,
        division, modulo, shift_right, shift_left, comparison_greater_than, array_access, array_access_wrapper,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, dereference, address_of,
        postfix_increment, prefix_decrement, postfix_decrement, prefix_increment_second, prefix_decrement_second bool_and, bool_or, ternary_selection }
        '*' $0 primary_expression
    ; < $x = LOAD $0
    >

address_of
    : [ '(', '['} { '.', '->' } ] primary_expression , ')', addition, subtraction, multiplication, method_call_no_args, method_call,
        division, modulo, shift_right, shift_left, comparison_greater_than, array_access, array_access_wrapper,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, dereference, address_of,
        postfix_increment, prefix_decrement, postfix_decrement, prefix_increment_second, prefix_decrement_second bool_and, bool_or, ternary_selection }
        '&' $0 primary_expression
    ; < $x = $0
    >

prefix_increment
    : [ '(', '['} { '.', '->' } '++' $0 primary_expression
    ; < $x = ADD loadable $0 1
    STORE $0 $x
    >

prefix_increment_second
    : [ '(', '['} { '.', '->' } $0 prefix_increment
    ; < $x = SUB loadable $0 1 
    >

postfix_increment
    : [ '(', '['} { '.', '->' } $0 primary_expression '++'
    ; < $x = ADD loadable $0 1 
    STORE $0 $x 
    >

prefix_decrement
    : [ '(', '['} { '.', '->' } '--' $0 primary_expression
    ; < $x = SUB loadable $0 1 
    STORE $0 $x 
    >

prefix_decrement_second
    : [ '(', '['} { '.', '->' } $0 prefix_decrement
    ; < $x = ADD loadable $0 1 
    >

postfix_decrement
    : [ '(', '['} { '.', '->' } $0 primary_expression '--'
    ; < $x = SUB loadable $0 1
    STORE $0 $x
    >

bool_and
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '++', '--' } ] '&&' }
    $0 primary_expression '&&' $1 primary_expression
    ; < $x = BOOL_AND loadable $0 loadable $1 
    >

bool_or
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '++', '--', '&&' } ] '||' }
    $0 primary_expression '||' $1 primary_expression
    ; < $x = BOOL_OR loadable $0 loadable $1 
    >

ternary_selection
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '++', '--', '&&', '||' } ] '?' }
    $0 primary_expression '?' $1 primary_expression ':' $2 primary_expression
    ; < $x = SELECT loadable $0 loadable $1 loadable $2 
    >

assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '++', '--', '&&', '||', '?', ':' } ] '=' }
    $0 primary_expression '=' $1 primary_expression
    ; < $x = loadable $1
    STORE $0 $x 
    >

add_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '++', '--', '&&', '||', '?', ':' } ] '+=' } 
    $0 primary_expression '+=' $1 primary_expression
    ; < $x = ADD loadable $0 loadable $1 
    STORE $0 $x 
    >

sub_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '++', '--', '&&', '||', '?', ':' } ] '-=' } 
    $0 primary_expression '-=' $1 primary_expression
    ; < $x = SUB loadable $0 loadable $1 
    STORE $0 $x 
    >

mul_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '++', '--', '&&', '||', '?', ':' } ] '*=' } 
    $0 primary_expression '*=' $1 primary_expression
    ; < $x = MUL loadable $0 loadable $1 
    STORE $0 $x 
    >

div_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '++', '--', '&&', '||', '?', ':' } ] '/=' } 
    $0 primary_expression '/=' $1 primary_expression
    ; < $x = DIV loadable $0 loadable $1 
    STORE $0 $x 
    >

mod_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '++', '--', '&&', '||', '?', ':' } ] '%=' } 
    $0 primary_expression '%=' $1 primary_expression
    ; < $x = MOD loadable $0 loadable $1 
    STORE $0 $x 
    >

shl_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '++', '--', '&&', '||', '?', ':' } ] '<<=' } 
    $0 primary_expression '<<=' $1 primary_expression
    ; < $x = SHL loadable $0 loadable $1 
    STORE $0 $x 
    >

shr_assign 
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '++', '--', '&&', '||', '?', ':' } ] '>>=' } 
    $0 primary_expression '>>=' $1 primary_expression
    ; < $x = SHR loadable $0 loadable $1 
    STORE $0 $x 
    >

and_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '++', '--', '&&', '||', '?', ':' } ] '&=' } 
    $0 primary_expression '&=' $1 primary_expression
    ; < $x = AND loadable $0 loadable $1 
    STORE $0 $x 
    >

xor_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '++', '--', '&&', '||', '?', ':' } ] '^=' } 
    $0 primary_expression '^=' $1 primary_expression
    ; < $x = XOR loadable $0 loadable $1 
    STORE $0 $x 
    >

or_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '^=', '++', '--', '&&', '||', '?', ':' } ] '|=' }
    $0 primary_expression '|=' $1 primary_expression
    ; < $x = OR loadable $0 loadable $1 
        STORE $0 $x 
    >

partial_scope
    : $0 '{' $1 primary_expression ';'
    | $0 '{' $1 statement
    | $0 '{' $1 scope
    | $0 '{' ';'
    | $0 partial_scope $1 primary_expression ';'
    | $0 partial_scope $1 statement
    | $0 partial_scope $1 scope
    | $0 partial_scope ';'
    ;

scope
    : $0 partial_scope '}'
    ; < scope_start $x
    !0 scope_end
    >

if_head
    : 'if' '(' $0 primary_expression ')'
    ;

if
    : [ 'else', else } $0 if_head $1 primary_expression ';'
    | [ 'else', else } $0 if_head $1 scope
    | [ 'else', else } $0 if_head ';'
    ; < JZ loadable $0 $x
        !1 $x :
    >

else
    : 'else' $0 primary_expression ';'
    | 'else' $0 scope
    | 'else' ';'
    ; < $x :
    >

if_else
    : $0 if_head $1 primary_expression ';' $2 else
    | $0 if_head $1 scope $2 else
    | $0 if_head $1 ';' $2 else
    ; < JZ loadable $0 $x
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

switch_case
    : $0 switch_case_opener $1 primary_expression ';'
    | $0 switch_case_opener $1 statement
    | $0 switch_case_opener $1 scope
    | $0 switch_case_opener ';'
    | $0 default_opener $1 primary_expression ';'
    | $0 default_opener $1 statement
    | $0 default_opener $1 scope
    | $0 default_opener ';'
    | $0 switch_case $1 primary_expression ';'
    | $0 switch_case $1 statement
    | $0 switch_case $1 scope
    | $0 switch_case ';'
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
    ; < continuable $x:
    >

while
    : $0 while_head $1 primary_expression $2 primary_expression ';'
    | ] ';', '}', scope } $0 while_head $1 primary_expression ';'
    | $0 while_head $1 primary_expression $2 scope
    | 'do' $2 primary_expression ';' $0 while_head $1 primary_expression ';'
    | 'do' $2 scope $0 while_head $1 primary_expression ';'
    ; < JZ loadable $1 $x 
        !2 JMP $0
        breakable $x :
    >

for_head
    : 'for' '(' $0 primary_expression ';'
    | 'for' '(' ';'
    ; < continuable $x:
    >

for
    : $0 for_head $1 primary_expression ';' $3 primary_expression ')' $2 primary_expression ';'
    | $0 for_head $1 primary_expression ';' $3 primary_expression ')' $2 statement
    | $0 for_head $1 primary_expression ';' $3 primary_expression ')' $2 scope
    | $0 for_head $1 primary_expression ';' $3 primary_expression ')' ';'
    ; < JZ loadable $1 $x
        !2 !3 JMP $0
        breakable $x : 
    >

for_empty_condition
    : $0 for_head ';' $2 primary_expression ')' $1 primary_expression ';'
    | $0 for_head ';' $2 primary_expression ')' $1 statement
    | $0 for_head ';' $2 primary_expression ')' $1 scope
    | $0 for_head ';' $2 primary_expression ')' ';'
    ; < JMP $0
        breakable $x :
    >

for_empty_final
    : $0 for_head $1 primary_expression ';' ')' $2 primary_expression ';'
    | $0 for_head $1 primary_expression ';' ')' $2 statement
    | $0 for_head $1 primary_expression ';' ')' $2 scope
    | $0 for_head $1 primary_expression ';' ')' ';'
    ; < JZ loadable $1 $x
        !2 JMP $0
        breakable $x :
    >

for_empty_all
    : $0 for_head ';' ')' $1 primary_expression ';'
    | $0 for_head ';' ')' $1 statement
    | $0 for_head ';' ')' $1 scope
    | $0 for_head ';' ')' ';'
    ; < JMP $0
        breakable $x :
    >

break
    : 'break' ';'
    ; < BREAK
    >

goto
    : 'goto' $0 primary_expression ';'
    ; < JMP goto_replace $0
    >

return
    : 'return' $0 primary_expression ';'
    ; < RET $0
    >

return_empty
    : 'return' ';'
    ; < RET void
    >

continue
    : 'continue' ';'
    ; < CONTINUE
    >

label
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '^=', '|=', '++', '--', '&&', '||', '?', ':' } 
    $0 primary_expression ':'
    ; < gotoable $0 $x :
    >


statement
    : $0 if
    | $0 if_else
    | $0 while
    | $0 break
    | $0 switch
    | $0 for
    | $0 for_empty_condition
    | $0 for_empty_final
    | $0 for_empty_all
    | $0 goto
    | $0 label
    | $0 return
    | $0 return_empty
    | $0 continue
    ;
)";