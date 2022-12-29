const char* c_yacc = R"(
piss: 
    $0 fuck 
    ;

fuck 
    : $1 ';' $0 '}' 
    ; < $x = ( DA $1 BASE )
    >

mew 
    : [piss} $0 piss $1 piss
    ; < $x = (DA $0 $1 JOIN)
    > 
    
mewmew
    : $0 mew $1 mew 
    ; < (DA OTHER JOIN $0 $1)
    >
)";