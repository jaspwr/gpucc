An prototype for a GPU based compiler for the C programming language using OpenGL compute shaders
to parse and compile C to x86_64 assembly. Using the GPU allows larger projects to be compiled in a
faster time than a traditional CPU based compiler.

In its current state, it is able to parse a subset of C that does not yet include things like structs, unions, enums or function pointers
and compile it to something very close to proper x86_64 assembly. As this project is more of a proof of concept
and I don't really have the time to work on it, it will likely never be finished.

## Basic Overview

The compilation is split into several stages, each comprised of one or more shaders (excluding the preprocessor).

1. ### Preprocessor (unfinished):
    The preprocessor is written in C++ and runs on the CPU.
1. ### Tokenizer:
    `src/shaders/tokeniser.glsl` takes in the input code in an SSBO and matches tokens with a TRIE like structure (this should be swapped for a simple hashmap). The file is split into 4 character chunks and each invocation looks through a chunk to see if any tokens start in that chunk and if so it will follow it through and match it to a token ID which it then adds to the `Tokens` SSBO which is the same size as the input code. The length of the tokens is also stored so that the empty space can be skipped when reading this SSBO later. This allows for large files to be tokenized in parallel and in theory means that the size of the file should not affect the run time.
2. ### AST
    `src/shaders/ast.glsl` takes the `Tokens` SSBO and splits it up similar to the tokenizer, giving chunks to each shader invocation which each then attempt to match patterns of tokens to the patterns in `src/grammars/c_grammar.cpp`. This file is in a YACC like syntax which is parsed by `src/yacc.cpp` (it is not actually YACC this naming is slightly bad). Once it matches a pattern, e.g. `IDENTIFIER '+' IDENTIFIER`, it will then replace the tokens in the `Tokens` SSBO with a new non-terminal token that represents that pattern and adds the corresponding node to the AST in the `AstNodes` SSBO. The length of the token is updated so that the empty space is skipped when reading later and the location of the AST node in the `AstNodes` SSBO is added to the token in the `Tokens` SSBO. This process is repeated until the AST is built i.e. the shader is rerun until the whole tree converges to one node.


    One issue we see with this type of parsing is that patterns like `'-' IDENTIFIER` will match to a unary minus pattern even when the full pattern might be `IDENTIFIER '-' IDENTIFIER` but the chunk given to that invocation cannot see the previous token. For this reason, each pattern has certain tokens that it checks for before and after the pattern that will block certain patterns from being matched.
3. ### IR Codegen
    `src/shaders/codegen.glsl` takes the AST and generates IR code in the `IrCodegen` SSBO. The patterns each AST node correspond to can be seen in `src/grammars/c_grammar.cpp` in the angel brackets after each match case.

    When building the AST, the total number of child nodes for each node is accumulated. This allows each invocation to find it's corresponding AST node in Post Order concurrantly completely independat of the others.

    Example: Invocation 2 is trying to find the 2nd node in Post Order.
```
               '+' [Volume: 5]  <---- A
                |
                /\   <---- B 
               /  \
              /    \
              |    '4' [Volume: 1]
              |    
 [Volume: 3] '*' <---- C
              |
             /\  <---- D
            /  \
           /    \
           |    |
           |   '2' [Volume: 1] <---- E
           |
          '6' [Volume: 1]
```
1. A variable called `workingVolume` is initialised as 2 as that is the node we are trying to find.
1. At point `A` it checks if `workingVolume` = the volume of the `'+'` node which it does not so it descends the tree.
1. A point `B`, we check if the volume is `<= workingVolume`. If that is the case then we can assume that that the node we are looking for in down the left branch we must descend the left branch otherwise we would descend the right branch and subtract the volumes of every branch to the left. This can be extended to more than two branches. As `workingVolume` (2) is not greater than the volume of the left branch (3), we descend the left most branch and subtract nothing from our working volume.
1. at point `C` we check if `workingVolume` = the volume of the `'+'` node which it does not so we descend.
1. At point `D`, the `workingVolume` (2) *is* greater than the volume of the left branch (1), so we decend the right branch subtracting 1 from `workingVolume`.
1. At point `E`, our `workingVolume` is equal to the volume of the `'2'` node so we have sucessfully found the second node in Post Order.

Addionally, a `codegenVolume` is accumulated so along with the other `volume` each node knows what position to codegen it's IR to (in the tree descending process instead of subtracting codegen volumes of left branches that are not descending it is accumulated). Each invocation then copies the IR into the `Output` SSBO. The invocation IDs are used at virtual register IDs. In the YACC `%X` denotes that it should be the virtual register with the ID of that node and the others refer the IDs of the child nodes. Also note that this IR is in SSA form.

1. ### Instruction selection
A syntax similar to the YACC in `src\arch\x86_64\intruction_selection.h` is used to define patterns in the IR that can be matched and then replaced with machine instructions. This is then performed by `src\shaders\instruction_selection.glsl`.

Example:
```
%a:i32 = ADD %b:i32 %c:i32 ->
    mov %a , %b
    add %a , %c
    ;

%a:f32 = ADD %b:f32 %c:f32 ->
    mov %a %b
    addss %a %c
    ;
```

1. ### Virtual register liveness analysis
The IR is divided up into segments where each invocation looks at a set of IR tokens. `src/shaders/live_intervals.glsl` then looks at all the references to virtual registers in it's segment and then will do an atomic max and atomic min of the position in the IR into the `LivenessIntervals` SSBO at the index of the virtual registers ID.

1. ### Register allocation
`src/shaders/register_allocator.glsl` is divided into two stages by an execution barrier. The first simply looks for any intersecting live ranges and creates a graph and the second will write allocated itself to a register greedily, then after an execution barrier check if any of it's neighbours also have the same register. If so, all but the register with the greatest ID will be evicted. This will be repeated until every register is allocated.

    
