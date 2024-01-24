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
    `src/shaders/ast.glsl` takes the `Tokens` SSBO and splits it up similar to the tokenizer, giving chunks to each shader invocation which each then attempt to match patterns of tokens to the patterns in `src/grammars/c_grammar.cpp`. This file is in a YACC like syntax which is parsed by `src/yacc.cpp` (it is not actually YACC this naming is slightly bad). Once it matches a pattern, e.g. `IDENTIFIER '+' IDENTIFIER`, it will then replace the tokens in the `Tokens` SSBO with a new non-terminal token that represents that pattern and adds the corresponding node to the AST in the `AstNodes` SSBO. The length of the token is updated so that the empty space is skipped when reading later and the location of the AST node in the `AstNodes` SSBO is added to the token in the `Tokens` SSBO. This process is repeated util the AST is built i.e. the shader is rerun until the whole tree converges to one node.


    One issue we see with this type of parsing is that patterns like `'-' IDENTIFIER` will match to a unary minus pattern even when the full pattern might be `IDENTIFIER '-' IDENTIFIER` but the chunk given to that invocation cannot see the previous token. For this reason, each pattern has certain tokens that it checks for before and after the pattern that will block certain patterns from being matched.
3. ### IR Codegen
    `src/shaders/codegen.glsl` takes the AST and generates IR code in the `IrCodegen` SSBO. The patterns each AST node correspond to can be seen in `src/grammars/c_grammar.cpp` in the angel brackets after each match case.

    When building the AST, the total number of child nodes for each node is accumulated. This allows each invocation to find it's corresponding