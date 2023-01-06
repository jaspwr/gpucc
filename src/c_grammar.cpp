const char* c_yacc = R"(

primary_expression
    : $0 { #identifier, type_specifier, type_qualifier } #identifier
    | $0 #literal
    | '(' $0 primary_expression ')'
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
    | $0 prefix_increment
    | $0 prefix_decrement
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
    ;

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
    : { '*', '/', '-', '%', '++', '--' } ] '+' } $0 primary_expression '+' $1 primary_expression
    ; < $x = ADD $0 $1 
    >

subtraction
    : { '*', '/', '%', '++', '--' } ] '-' } $0 primary_expression '-' $1 primary_expression
    ; < $x = SUB $0 $1 
    >

multiplication
    : { '/', '%', '++', '--' } ] '*' } $0 primary_expression '*' $1 primary_expression
    ; < $x = MUL $0 $1 
    >

division
    : [ '++', '--' } ] '/' } $0 primary_expression '/' $1 primary_expression
    ; < $x = DIV $0 $1 
    >

modulo
    : ] '%', '++', '--' } $0 primary_expression '%' $1 primary_expression
    ; < $x = MOD $0 $1 
    >

shift_right
    : { '*', '/', '%', '-', '+', '<<', '++', '--' } ] '>>' } $0 primary_expression '>>' $1 primary_expression
    ; < $x = SHR $0 $1 
    >

shift_left
    : { '*', '/', '%', '-', '+', '++', '--' } ] '<<' } $0 primary_expression '<<' $1 primary_expression
    ; < $x = SHL $0 $1 
    >

comparison_less_than
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', '++', '--' } ] '<' } $0 primary_expression '<' $1 primary_expression
    ; < $x = CMP LT $0 $1 
    >

comparison_greater_than
    : { '*', '/', '%', '-', '+', '<<', '>>', '++', '--' } ] '>' } $0 primary_expression '>' $1 primary_expression
    ; < $x = CMP GT $0 $1 
    >

comparison_less_than_or_equal
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '>=', '++', '--' } ] '<=' } $0 primary_expression '<=' $1 primary_expression
    ; < $x = CMP LTE $0 $1 
    >

comparison_greater_than_or_equal
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '++', '--' } ] '>=' } $0 primary_expression '>=' $1 primary_expression
    ; < $x = CMP GTE $0 $1 
    >

comparison_equal
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '!=', '++', '--' } ] '==' } $0 primary_expression '==' $1 primary_expression
    ; < $x = CMP EQ $0 $1 
    >

comparison_not_equal
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '++', '--' } ] '!=' } $0 primary_expression '!=' $1 primary_expression
    ; < $x = CMP NE $0 $1 
    >

bitwise_and
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '!=', '==', '++', '--' } ] '&' } $0 primary_expression '&' $1 primary_expression
    ; < $x = AND $0 $1 
    >

bitwise_xor
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '!=', '==', '&', '++', '--' } ] '^' } $0 primary_expression '^' $1 primary_expression
    ; < $x = XOR $0 $1
    >

bitwise_or
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '!=', '==', '&', '^', '++', '--' } ] '|' } $0 primary_expression '|' $1 primary_expression
    ; < $x = OR $0 $1 
    >

unary_plus
    : ] primary_expression , ')', addition, subtraction, multiplication, 
        division, modulo, shift_right, shift_left, comparison_greater_than,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, 
        postfix_increment, prefix_decrement, postfix_decrement, bool_and, bool_or, ternary_selection }
        '+' $0 primary_expression
    ;

unary_minus
    : ] primary_expression , ')', addition, subtraction, multiplication, 
        division, modulo, shift_right, shift_left, comparison_greater_than,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, 
        postfix_increment, prefix_decrement, postfix_decrement, bool_and, bool_or, ternary_selection }
        '-' $0 primary_expression
    ; < $x = SUB 0 $0 
    >

unary_not
    : ] primary_expression , ')', addition, subtraction, multiplication, 
        division, modulo, shift_right, shift_left, comparison_greater_than,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, 
        postfix_increment, prefix_decrement, postfix_decrement, bool_and, bool_or, ternary_selection }
        '!' $0 primary_expression
    ; < $x = BOOL_NOT $0 
    >

unary_bitwise_not
    : ] primary_expression , ')', addition, subtraction, multiplication, 
        division, modulo, shift_right, shift_left, comparison_greater_than,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, 
        postfix_increment, prefix_decrement, postfix_decrement, bool_and, bool_or, ternary_selection }
        '~' $0 primary_expression
    ; < $x = XOR $0 -1
    >

prefix_increment
    : ] primary_expression , ')', addition, subtraction, multiplication, 
        division, modulo, shift_right, shift_left, comparison_greater_than,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, 
        postfix_increment, prefix_decrement, postfix_decrement, bool_and, bool_or, ternary_selection }
        '++' $0 primary_expression
    ; < $x = ADD $0 1
    STORE $0 $x
    $x = $0
    >

postfix_increment
    : ] primary_expression , ')', addition, subtraction, multiplication, 
        division, modulo, shift_right, shift_left, comparison_greater_than,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, 
        postfix_increment, prefix_decrement, postfix_decrement, bool_and, bool_or, ternary_selection }
        $0 primary_expression '++'
    ; < $x = ADD $0 1
    STORE $0 $x
    >

prefix_decrement
    : ] primary_expression , ')', addition, subtraction, multiplication, 
        division, modulo, shift_right, shift_left, comparison_greater_than,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, 
        postfix_increment, prefix_decrement, postfix_decrement, bool_and, bool_or, ternary_selection }
        '--' $0 primary_expression
    ; < $x = SUB $0 1
    STORE $0 $x
    $x = $0
    >

postfix_decrement
    : ] primary_expression , ')', addition, subtraction, multiplication, 
        division, modulo, shift_right, shift_left, comparison_greater_than,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not, 
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, 
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, 
        postfix_increment, prefix_decrement, postfix_decrement, bool_and, bool_or, ternary_selection }
        $0 primary_expression '--'
    ; < $x = SUB $0 1
    STORE $0 $x
    >

bool_and
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '++', '--' } ] '&&' }
    $0 primary_expression '&&' $1 primary_expression
    ; < $x = BOOL_AND $0 $1 
    >

bool_or
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '++', '--', '&&' } ] '||' }
    $0 primary_expression '||' $1 primary_expression
    ; < $x = BOOL_OR $0 $1 
    >

ternary_selection
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '++', '--', '&&', '||' } ] '?' }
    $0 primary_expression '?' $1 primary_expression ':' $2 primary_expression
    ; < $x = SELECT $0 $1 $2 
    >

assign
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '++', '--', '&&', '||', '?', ':' } ] '=' }
    $0 primary_expression '=' $1 primary_expression
    ; < $x = $1
    STORE $0 $x 
    >

add_assign
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '++', '--', '&&', '||', '?', ':' } ] '+=' } 
    $0 primary_expression '+=' $1 primary_expression
    ; < $x = ADD $0 $1 
    STORE $0 $x 
    >

sub_assign
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '++', '--', '&&', '||', '?', ':' } ] '-=' } 
    $0 primary_expression '-=' $1 primary_expression
    ; < $x = SUB $0 $1 
    STORE $0 $x 
    >

mul_assign
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '++', '--', '&&', '||', '?', ':' } ] '*=' } 
    $0 primary_expression '*=' $1 primary_expression
    ; < $x = MUL $0 $1 
    STORE $0 $x 
    >

div_assign
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '++', '--', '&&', '||', '?', ':' } ] '/=' } 
    $0 primary_expression '/=' $1 primary_expression
    ; < $x = DIV $0 $1 
    STORE $0 $x 
    >

mod_assign
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '++', '--', '&&', '||', '?', ':' } ] '%=' } 
    $0 primary_expression '%=' $1 primary_expression
    ; < $x = MOD $0 $1 
    STORE $0 $x 
    >

shl_assign
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '++', '--', '&&', '||', '?', ':' } ] '<<=' } 
    $0 primary_expression '<<=' $1 primary_expression
    ; < $x = SHL $0 $1 
    STORE $0 $x 
    >

shr_assign 
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '++', '--', '&&', '||', '?', ':' } ] '>>=' } 
    $0 primary_expression '>>=' $1 primary_expression
    ; < $x = SHR $0 $1 
    STORE $0 $x 
    >

and_assign
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '++', '--', '&&', '||', '?', ':' } ] '&=' } 
    $0 primary_expression '&=' $1 primary_expression
    ; < $x = AND $0 $1 
    STORE $0 $x 
    >

xor_assign
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '++', '--', '&&', '||', '?', ':' } ] '^=' } 
    $0 primary_expression '^=' $1 primary_expression
    ; < $x = XOR $0 $1 
    STORE $0 $x 
    >

or_assign
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', 
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '^=', '++', '--', '&&', '||', '?', ':' } ] '|=' }
    $0 primary_expression '|=' $1 primary_expression
    ; < $x = OR $0 $1 
    STORE $0 $x 
    >

scope_opener
    : '{'
    ; < $x : 
    >

partial_scope
    : $0 scope_opener $1 primary_expression ';'
    | $0 partial_scope $1 primary_expression ';'
    ;

scope
    : $0 partial_scope '}'
    ;

)";