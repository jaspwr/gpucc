// Copyright (C) 2023 Jasper Parker <j@sperp.dev>
//
// This file is part of Meow.
//
// Meow is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// Meow is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Meow. If not, see <https://www.gnu.org/licenses/>.


const char* c_pre_yacc = R"(
primary_expression
    : $0 { #identifier, type_specifier, type_qualifier, struct_union_type
    'int', 'float', 'char', 'void', 'double' 'long' 'struct', 'union', 'enum' , int, char, float, void } #identifier
    | $0 #literal
    | ] 'if', 'switch', 'for' } '(' $0 primary_expression ')'
    | $0 sizeof
    | $0 access
    ;

switch_case_opener
    : 'case' $0 primary_expression ':'
    ; < switch_case $0 $x :
    >

default_opener
    : 'default' ':'
    ; < switch_default $x :
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
    | $0 double
    | $0 void
    | [ '{' } $0 enum_type
    | [ ';' } $0 full_struct_definition
    | [ ';' } $0 full_enum_definition
    ;

int : 'int' ;
char : 'char' ;
float : 'float' ;
double : 'double' ;
void : 'void' ;
long : 'long' ;
short : 'short' ;
signed : 'signed' ;
unsigned : 'unsigned' ;
const : 'const' ;
volatile : 'volatile' ;
register : 'register' ;

pointer
    : $0 type_specifier '*'
    | $0 pointer '*'
    | $0 struct_union_type '*'
    ;

struct_union_type
    : $0 union_type
    | $0 struct_type
    ;

struct_type
    : 'struct' $0 #identifier
    ; < struct $0 >

struct_definition_head
    : $0 struct_type '{'
    | $0 union_type '{'
    ; < !0 { >

unnamed_struct_definition_head
    : $0 'struct' '{'
    ; < struct { >

unnamed_union_definition_head
    : $0 'union' '{'
    ; < union { >

partial_struct_definition
    : $0 struct_definition_head $1 declaration ';'
    | $0 unnamed_struct_definition_head $1 declaration ';'
    | $0 unnamed_union_definition_head $1 declaration ';'
    | $0 partial_struct_definition $1 declaration ';'
    ; < !0 !1 >

full_struct_definition
    : $0 partial_struct_definition '}'
    ; < !0 } >

union_type
    : 'union' $0 #identifier
    ; < union $0 >

enum_type
    : 'enum' #identifier
    ; < enum >

enum_definition_head
    : 'enum' '{'
    | enum_type '{'
    ; < enum { >

partial_enum_definition
    : $0 enum_definition_head $1 primary_expression ','
    | [ '=' } $0 enum_definition_head $1 primary_expression
    | $0 partial_enum_definition $1 primary_expression ','
    | [ '=' } $0 partial_enum_definition $1 primary_expression
    | $0 partial_enum_definition_assign $1 primary_expression ','
    | [ '=' } $0 partial_enum_definition_assign $1 primary_expression
    ; < !0 $1 >

partial_enum_definition_assign
    : $0 enum_definition_head $1 primary_expression '=' $2 primary_expression ','
    | $0 enum_definition_head $1 primary_expression '=' $2 primary_expression
    | $0 partial_enum_definition_assign $1 primary_expression '=' $2 primary_expression ','
    | $0 partial_enum_definition_assign $1 primary_expression '=' $2 primary_expression
    | $0 partial_enum_definition $1 primary_expression '=' $2 primary_expression ','
    | $0 partial_enum_definition $1 primary_expression '=' $2 primary_expression
    ; < !0 set_counter $2 $1 >

full_enum_definition
    : $0 enum_definition_head '}'
    | $0 partial_enum_definition '}'
    | $0 partial_enum_definition_assign '}'
    ; < !0 } >

type_qualifier
    : $0 const
    | $0 volatile
    | $0 unsigned
    | $0 long
    | $0 signed
    | $0 short
    | $0 register
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

array_pointer
    : [ '[' } '[' ']'
    | '[' ']' $0 array_pointer
    ; < ptr !0 >

declaration
    : ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $1 type_specifier $2 #identifier
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $1 type_specifier $2 primary_expression
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $1 struct_union_type $2 #identifier
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $1 struct_union_type $2 primary_expression
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $1 pointer $2 primary_expression
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $0 qualifier_list $1 type_specifier $2 #identifier
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $0 qualifier_list $1 struct_union_type $2 #identifier
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $0 qualifier_list $1 type_specifier $2 primary_expression
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $0 qualifier_list $1 struct_union_type $2 primary_expression
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $0 qualifier_list $1 pointer $2 primary_expression
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $1 type_specifier $2 #identifier $3 array_pointer
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $1 type_specifier $2 primary_expression $3 array_pointer
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $1 struct_union_type $2 #identifier $3 array_pointer
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $1 struct_union_type $2 primary_expression $3 array_pointer
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $1 pointer $2 primary_expression $3 array_pointer
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $0 qualifier_list $1 type_specifier $2 #identifier $3 array_pointer
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $0 qualifier_list $1 struct_union_type $2 #identifier $3 array_pointer
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $0 qualifier_list $1 type_specifier $2 primary_expression $3 array_pointer
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $0 qualifier_list $1 struct_union_type $2 primary_expression $3 array_pointer
    | ] type_qualifier, qualifier_list, #identifier } [ '[', array_pointer } $0 qualifier_list $1 pointer $2 primary_expression $3 array_pointer
    ; < $2 >

declaration_list
    : $0 declaration_list $1 declaration ','
    | ] ',', declaration_list } $0 declaration ','
    ; < , fn_def_arg >

function_definition_head
    : $0 declaration '(' $1 declaration_list $2 declaration ')'
    | $0 declaration '(' $1 declaration ')'
    | $0 declaration '(' ')'
    ; < fn !0 ( fn_def_arg !1 !2 ) >

method_call_head
    : $0 primary_expression '('
    ;

access
    : $0 primary_expression '.' $1 primary_expression
    | $0 primary_expression '->' $1 primary_expression
    ; < $x = GETELEMENTPTR $0 $1
    >
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
    | $0 dereference
    | $0 address_of
    | $0 cast_expression
    | $0 access
    | $0 comma
    ;

function_definition
    : $0 function_definition_head $1 scope
    ; < !0 FN_PRELUDE
        !1RET
    >

global_statement
    : $0 function_definition
    | $0 full_struct_definition ';'
    | $0 full_enum_definition ';'
    ; < !0
    >

compound_global_statement
    : ] global_statement } $0 global_statement $1 global_statement
    | $0 compound_global_statement $1 global_statement
    | ] compound_global_statement } $0 compound_global_statement $1 compound_global_statement
    ;

declaration_full
    : $0 declaration
    ; < $x = ALLOCA !0
    >

declaration_assign
    : $0 declaration_full '=' $1 primary_expression ';'
    ; < $x = COPY $1
    STORE $0 $x
    ` $x := $1 ` >

declaration_nothing
    : $0 declaration_full ';'
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
    ; < $x = COPY $0
    >

argument_list_open
    : [ '(', '['} { '.', '->' } [ '*', '/', '%', '-', '+', '<<', '>>', '>',
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '^=', '|=', '++', '--', '&&', '||', '?', ':' }
    $0 method_call_head $1 primary_expression
    ; < replace_me = CALL $0 ( $1 >

argument_list
    : [ '(', '['} { '.', '->' } [ '*', '/', '%', '-', '+', '<<', '>>', '>',
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '^=', '|=', '++', '--', '&&', '||', '?', ':' }
    $1 argument_list_open ',' $0 primary_expression
    | $1 argument_list ',' $0 primary_expression
    ; < , $0 >

comma
    : ] ',', method_call_head } [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>',
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '^=', '|=', '++', '--', '&&', '||', '?', ':' }
     $0 primary_expression ',' $1 primary_expression
    ; < $x = COPY $1
    ` $x := $1 ` >

addition
    : [ '(', '['} { '.', '->' } { '*', '/', '-', '%', '++', '--' } ] '+' } $0 primary_expression '+' $1 primary_expression
    ; < $x = ADD $0 $1
    ` $x := $0 ` >

subtraction
    : [ '(', '['} { '.', '->' } { '*', '/', '%', '++', '--' } ] '-' } $0 primary_expression '-' $1 primary_expression
    ; < $x = SUB $0 $1
    >

multiplication
    : [ '(', '['} { '.', '->' } { '/', '%', '++', '--' } ] '*' } $0 primary_expression '*' $1 primary_expression
    ; < $x = MUL $0 $1
    ` $x := $0 | $1 ` >

division
    : [ '(', '['} { '.', '->' } [ '++', '--' } ] '/' } $0 primary_expression '/' $1 primary_expression
    ; < $x = DIV $0 $1
    >

modulo
    : [ '(', '['} { '.', '->' } ] '%', '++', '--' } $0 primary_expression '%' $1 primary_expression
    ; < $x = MOD $0 $1
    >

shift_right
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '++', '--' } ] '>>' } $0 primary_expression '>>' $1 primary_expression
    ; < $x = SHR $0 $1
    >

shift_left
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '++', '--' } ] '<<' } $0 primary_expression '<<' $1 primary_expression
    ; < $x = SHL $0 $1
    >

comparison_less_than
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', '++', '--' } ] '<' } $0 primary_expression '<' $1 primary_expression
    ; < $x = CMP LT $0 $1
    `$x := i1` >

comparison_greater_than
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '++', '--' } ] '>' } $0 primary_expression '>' $1 primary_expression
    ; < $x = CMP GT $0 $1
    >

comparison_less_than_or_equal
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '>=', '++', '--' } ] '<=' } $0 primary_expression '<=' $1 primary_expression
    ; < $x = CMP LTE $0 $1
    >

comparison_greater_than_or_equal
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '++', '--' } ] '>=' } $0 primary_expression '>=' $1 primary_expression
    ; < $x = CMP GTE $0 $1
    >

comparison_equal
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '!=', '++', '--' } ] '==' } $0 primary_expression '==' $1 primary_expression
    ; < $x = CMP EQ $0 $1
    ` $x := i1 ` >

comparison_not_equal
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '++', '--' } ] '!=' } $0 primary_expression '!=' $1 primary_expression
    ; < $x = CMP NE $0 $1
    >

bitwise_and
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '!=', '==', '++', '--' } ] '&' } $0 primary_expression '&' $1 primary_expression
    ; < $x = AND $0 $1
    >

bitwise_xor
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '!=', '==', '&', '++', '--' } ] '^' } $0 primary_expression '^' $1 primary_expression
    ; < $x = XOR $0 $1
    >

bitwise_or
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '!=', '==', '&', '^', '++', '--' } ] '|' } $0 primary_expression '|' $1 primary_expression
    ; < $x = OR $0 $1
    >

cast_expression
    : [ '(', '['} { '.', '->' } [ '++', '--' }
    $1 cast_operator $0 primary_expression
    ; < $x = CAST !1 $0
    >

unary_plus
    : [ '(', '['} { '.', '->' } ] primary_expression , ')', addition, subtraction, multiplication, method_call_no_args, method_call,
        division, modulo, shift_right, shift_left, comparison_greater_than, array_access, array_access_wrapper,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not,
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, comma,
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, dereference, address_of,
        postfix_increment, prefix_decrement, postfix_decrement, prefix_increment_second, prefix_decrement_second bool_and, bool_or, ternary_selection }
        '+' $0 primary_expression
    ;

unary_minus
    : [ '(', '['} { '.', '->' } ] primary_expression , ')', addition, subtraction, multiplication, method_call_no_args, method_call,
        division, modulo, shift_right, shift_left, comparison_greater_than, array_access, array_access_wrapper,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not,
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, comma,
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, dereference, address_of,
        postfix_increment, prefix_decrement, postfix_decrement, prefix_increment_second, prefix_decrement_second bool_and, bool_or, ternary_selection }
        '-' $0 primary_expression
    ; < $x = SUB 0 $0
    >

unary_not
    : [ '(', '['} { '.', '->' } ] primary_expression , ')', addition, subtraction, multiplication, method_call_no_args, method_call,
        division, modulo, shift_right, shift_left, comparison_greater_than, array_access, array_access_wrapper,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not,
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, comma,
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, dereference, address_of,
        postfix_increment, prefix_decrement, postfix_decrement, prefix_increment_second, prefix_decrement_second bool_and, bool_or, ternary_selection }
        '!' $0 primary_expression
    ; < $x = XOR $0 1
    >

unary_bitwise_not
    : [ '(', '['} { '.', '->' } ] primary_expression , ')', addition, subtraction, multiplication, method_call_no_args, method_call,
        division, modulo, shift_right, shift_left, comparison_greater_than, array_access, array_access_wrapper,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not,
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, comma,
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, dereference, address_of,
        postfix_increment, prefix_decrement, postfix_decrement, prefix_increment_second, prefix_decrement_second bool_and, bool_or, ternary_selection }
        '~' $0 primary_expression
    ; < $x = XOR $0 -1
    >

dereference
    : [ '(', '['} { '.', '->' } ] primary_expression , ')', addition, subtraction, multiplication, method_call_no_args, method_call,
        division, modulo, shift_right, shift_left, comparison_greater_than, array_access, array_access_wrapper,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal
        comparison_equal, comparison_not_equal, bitwise_and, bitwise_xor, bitwise_or ,unary_not,
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, comma,
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
        unary_bitwise_not, assign, add_assign, sub_assign, mul_assign, div_assign, mod_assign, comma,
        shl_assign, shr_assign, and_assign, xor_assign, or_assign, prefix_increment, dereference, address_of,
        postfix_increment, prefix_decrement, postfix_decrement, prefix_increment_second, prefix_decrement_second bool_and, bool_or, ternary_selection }
        '&' $0 primary_expression
    ; < $x = $0
    >

prefix_increment
    : [ '(', '['} { '.', '->' } '++' $0 primary_expression
    ; < $x = ADD $0 1
    STORE $0 $x
    >

prefix_increment_second
    : [ '(', '['} { '.', '->' } $0 prefix_increment
    ; < $x = SUB $0 1
    >

postfix_increment
    : [ '(', '['} { '.', '->' } $0 primary_expression '++'
    ; < $x = ADD $0 1
    STORE $0 $x
    >

prefix_decrement
    : [ '(', '['} { '.', '->' } '--' $0 primary_expression
    ; < $x = SUB $0 1
    STORE $0 $x
    >

prefix_decrement_second
    : [ '(', '['} { '.', '->' } $0 prefix_decrement
    ; < $x = ADD $0 1
    >

postfix_decrement
    : [ '(', '['} { '.', '->' } $0 primary_expression '--'
    ; < $x = SUB $0 1
    STORE $0 $x
    >

bool_and
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>',
    '<', '<=', '>=', '!=', '==', '&', '^', '++', '--' } ] '&&' }
    $0 primary_expression '&&' $1 primary_expression
    ; < $x = BOOL_AND $0 $1
    >

bool_or
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>',
    '<', '<=', '>=', '!=', '==', '&', '^', '++', '--', '&&' } ] '||' }
    $0 primary_expression '||' $1 primary_expression
    ; < $x = BOOL_OR $0 $1
    >

ternary_selection
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>',
    '<', '<=', '>=', '!=', '==', '&', '^', '++', '--', '&&', '||' } ] '?' }
    $0 primary_expression '?' $1 primary_expression ':' $2 primary_expression
    ; < $x = SELECT $0 $1 $2
    >

assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>',
    '<', '<=', '>=', '!=', '==', '&', '^', '++', '--', '&&', '||', '?', ':' } ] '=' }
    $0 primary_expression '=' $1 primary_expression
    ; < $x = COPY $1
    STORE $0 $x
    ` $x := $1 ` >

add_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>',
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '++', '--', '&&', '||', '?', ':' } ] '+=' }
    $0 primary_expression '+=' $1 primary_expression
    ; < $x = ADD $0 $1
    STORE $0 $x
    ` $x := $1 ` >

sub_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>',
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '++', '--', '&&', '||', '?', ':' } ] '-=' }
    $0 primary_expression '-=' $1 primary_expression
    ; < $x = SUB $0 $1
    STORE $0 $x
    >

mul_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>',
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '++', '--', '&&', '||', '?', ':' } ] '*=' }
    $0 primary_expression '*=' $1 primary_expression
    ; < $x = MUL $0 $1
    STORE $0 $x
    >

div_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>',
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '++', '--', '&&', '||', '?', ':' } ] '/=' }
    $0 primary_expression '/=' $1 primary_expression
    ; < $x = DIV $0 $1
    STORE $0 $x
    >

mod_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>',
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '++', '--', '&&', '||', '?', ':' } ] '%=' }
    $0 primary_expression '%=' $1 primary_expression
    ; < $x = MOD $0 $1
    STORE $0 $x
    >

shl_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>',
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '++', '--', '&&', '||', '?', ':' } ] '<<=' }
    $0 primary_expression '<<=' $1 primary_expression
    ; < $x = SHL $0 $1
    STORE $0 $x
    >

shr_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>',
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '++', '--', '&&', '||', '?', ':' } ] '>>=' }
    $0 primary_expression '>>=' $1 primary_expression
    ; < $x = SHR $0 $1
    STORE $0 $x
    >

and_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>',
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '++', '--', '&&', '||', '?', ':' } ] '&=' }
    $0 primary_expression '&=' $1 primary_expression
    ; < $x = AND $0 $1
    STORE $0 $x
    >

xor_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>',
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '++', '--', '&&', '||', '?', ':' } ] '^=' }
    $0 primary_expression '^=' $1 primary_expression
    ; < $x = XOR $0 $1
    STORE $0 $x
    >

or_assign
    : [ '(', '['} { '.', '->' } { cast_operator, '*', '/', '%', '-', '+', '<<', '>>', '>',
    '<', '<=', '>=', '!=', '==', '&', '^', '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '^=', '++', '--', '&&', '||', '?', ':' } ] '|=' }
    $0 primary_expression '|=' $1 primary_expression
    ; < $x = OR $0 $1
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
    | $0 partial_scope_dec $1 primary_expression ';'
    | $0 partial_scope_dec $1 statement
    | $0 partial_scope_dec $1 scope
    | $0 partial_scope_dec ';'
    ;

partial_scope_dec
    : $0 '{' $1 declaration_assign
    | $0 '{' $1 declaration_nothing
    | $0 partial_scope $1 declaration_assign
    | $0 partial_scope $1 declaration_nothing
    | $0 partial_scope_dec $1 declaration_assign
    | $0 partial_scope_dec $1 declaration_nothing
    ;

scope
    : $0 partial_scope '}'
    | $0 partial_scope_dec '}'
    | '{' '}'
    ;

if_head
    : 'if' '(' $0 primary_expression ')'
    ;

if
    : [ 'else', else } $0 if_head $1 primary_expression ';'
    | [ 'else', else } $0 if_head $1 scope
    | [ 'else', else } $0 if_head ';'
    ; < JZ $0 $x
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
    ; < JZ $0 $x
        !1 JMP $2
        $x :
        !2 >

switch_head
    : 'switch' '(' $0 primary_expression ')'
    ;

breakable
    : $0 switch
    | $0 continuable
    | $0 for_wrapper
    ; < $x :
    >

for_wrapper
    : $0 for
    | $0 for_empty_condition
    | $0 for_empty_final
    | $0 for_empty_all
    ;

continuable
    : $0 while
    ; < $x :
    !0 >

switch
    : $0 switch_head $1 switch_body
    ; < switch $0 [
    ]
        !1 switch_body_end
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


while
    : 'while' $1 primary_expression $2 primary_expression ';'
    | ] ';', '}', scope } 'while' $1 primary_expression ';'
    | 'while' $1 primary_expression $2 scope
    | 'do' $2 primary_expression ';' 'while' $1 primary_expression ';'
    | 'do' $2 scope $0 'while' $1 primary_expression ';'
    ; < JZ $1 BREAK NOP
        !2 JMP CONTINUE NOP
    >

for_head
    : 'for' '(' $0 primary_expression ';'
    | 'for' '(' $0 declaration_assign
    | 'for' '(' $0 declaration_nothing
    | 'for' '(' ';'
    ; < $x:
    >

for
    : $0 for_head $1 primary_expression ';' $3 primary_expression ')' $2 primary_expression ';'
    | $0 for_head $1 primary_expression ';' $3 primary_expression ')' $2 statement
    | $0 for_head $1 primary_expression ';' $3 primary_expression ')' $2 scope
    | $0 for_head $1 primary_expression ';' $3 primary_expression ')' ';'
    ; < JZ $1 BREAK NOP
        !2 $x:
        !3 JMP $0
    >

for_empty_condition
    : $0 for_head ';' $2 primary_expression ')' $1 primary_expression ';'
    | $0 for_head ';' $2 primary_expression ')' $1 statement
    | $0 for_head ';' $2 primary_expression ')' $1 scope
    | $0 for_head ';' $2 primary_expression ')' ';'
    ; < $x:
    JMP $0
    >

for_empty_final
    : $0 for_head $1 primary_expression ';' ')' $2 primary_expression ';'
    | $0 for_head $1 primary_expression ';' ')' $2 statement
    | $0 for_head $1 primary_expression ';' ')' $2 scope
    | $0 for_head $1 primary_expression ';' ')' ';'
    ; < JZ $1 BREAK NOP
        !2 $x:
        JMP $0
    >

for_empty_all
    : $0 for_head ';' ')' $1 primary_expression ';'
    | $0 for_head ';' ')' $1 statement
    | $0 for_head ';' ')' $1 scope
    | $0 for_head ';' ')' ';'
    ; < $x:
    JMP $0
    >

break
    : 'break' ';'
    ; < JMP BREAK NOP
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
    ; < JMP CONTINUE NOP
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
    | $0 break
    | $0 breakable
    | $0 goto
    | $0 label
    | $0 return
    | $0 return_empty
    | $0 continue
    | ] statement } $0 statement $1 statement
    ;
)";
