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
    ;

type_specifier
    : $0 'int'
    | $0 'float'
    | $0 'void'
    | $0 'char'
    ;

type_qualifier
    : $0 'const'
    | $0 'volatile'
    ;

qualifier_list
    : ] type_qualifier } $0 type_qualifier
    | $1 qualifier_list $0 type_qualifier
    ; < $0 >


addition
    : { '*', '/', '-', '%' } ] '+' } $0 primary_expression '+' $1 primary_expression
    ; < $x = ADD $0 $1 
    >

subtraction
    : { '*', '/', '%' } ] '-' } $0 primary_expression '-' $1 primary_expression
    ; < $x = SUB $0 $1 
    >

multiplication
    : { '/', '%' } ] '*' } $0 primary_expression '*' $1 primary_expression
    ; < $x = MUL $0 $1 
    >

division
    : $0 primary_expression '/' $1 primary_expression
    ; < $x = DIV $0 $1 
    >

modulo
    : ] '%' } $0 primary_expression '%' $1 primary_expression
    ; < $x = MOD $0 $1 
    >

shift_right
    : { '*', '/', '%', '-', '+', '<<' } ] '>>' } $0 primary_expression '>>' $1 primary_expression
    ; < $x = SHR $0 $1 
    >

shift_left
    : { '*', '/', '%', '-', '+' } ] '<<' } $0 primary_expression '<<' $1 primary_expression
    ; < $x = SHL $0 $1 
    >

comparison_less_than
    : { '*', '/', '%', '-', '+', '<<', '>>', '>' } ] '<' } $0 primary_expression '<' $1 primary_expression
    ; < $x = CMP LT $0 $1 
    >

comparison_greater_than
    : { '*', '/', '%', '-', '+', '<<', '>>' } ] '>' } $0 primary_expression '>' $1 primary_expression
    ; < $x = CMP GT $0 $1 
    >

comparison_less_than_or_equal
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '>=' } ] '<=' } $0 primary_expression '<=' $1 primary_expression
    ; < $x = CMP LTE $0 $1 
    >

comparison_greater_than_or_equal
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', '<' } ] '>=' } $0 primary_expression '>=' $1 primary_expression
    ; < $x = CMP GTE $0 $1 
    >

comparison_equal
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=', '!=' } ] '==' } $0 primary_expression '==' $1 primary_expression
    ; < $x = CMP EQ $0 $1 
    >

comparison_not_equal
    : { '*', '/', '%', '-', '+', '<<', '>>', '>', '<', '<=', '>=' } ] '!=' } $0 primary_expression '!=' $1 primary_expression
    ; < $x = CMP NE $0 $1 
    >

unary_plus
    : ] primary_expression , ')', addition, subtraction, multiplication, 
        division, modulo, shift_right, shift_left, comparison_greater_than,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal }
        '+' $0 primary_expression
    ;

unary_minus
    : ] primary_expression , ')', addition, subtraction, multiplication, 
        division, modulo, shift_right, shift_left, comparison_greater_than,
        comparison_less_than, comparison_less_than_or_equal, comparison_greater_than_or_equal 
        comparison_equal, comparison_not_equal }
        '-' $0 primary_expression
    ; < $x = SUB 0 $0 
    >

assignment_expression
    : $0 primary_expression '=' $1 primary_expression ';'
    ; < STORE $0 $1 
    >

scope_opener
    : '{'
    ; < $x : 
    >

partial_scope
    : $0 scope_opener $1 primary_expression ';'
    | $0 scope_opener $1 assignment_expression
    | $0 partial_scope $1 primary_expression ';'
    | $0 partial_scope $1 assignment_expression
    ;

scope
    : $0 partial_scope '}'
    ;

)";