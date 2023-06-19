const char* type_propagation = R"(
%_:t1 = ADD %_:t2 %_:t3 ->
    t1 := t2 | t3
    ;

%_:t1 = MUL %_:t2 %_:t3 ->
    t1 := t2 | t3
    ;

%_:t1 = SUB %_:t2 %_:t3 ->
    t1 := t2 | t3
    ;

%_:t1 = DIV %_:t2 %_:t3 ->
    t1 := t2 | t3
    ;

%_:t1 = DIV %_:t2 %_:t3 ->
    t1 := t2 | t3
    ;




%_:t1 = LOAD %_:t2_ptr ->
    t1 := t2
    ;
)";

