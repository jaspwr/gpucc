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

unary_plus
    : ] primary_expression , ')', addition, subtraction, multiplication, division, modulo } '+' $0 primary_expression
    ;

unary_minus
    : ] primary_expression, ')', addition, subtraction, multiplication, division, modulo } '-' $0 primary_expression
    ; < $x = SUB 0 $0 
    >

assignment_expression
    : $0 primary_expression '=' $1 primary_expression ';'
    ; < STORE $0 $1 
    >

partial_scope
    : '{' $0 primary_expression ';'
    | '{' $0 assignment_expression
    | $0 partial_scope $1 primary_expression ';'
    | $0 partial_scope $1 assignment_expression
    ;

scope
    : $0 partial_scope '}'
    ; < $x : 
    >

)";