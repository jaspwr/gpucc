An prototype for a GPU based compiler for the C programming language using OpenGL compute shaders
to parse and compile C to x86_64 assembly. Using the GPU allows larger projects to be compiled in a
faster time than a traditional CPU based compiler.

In its current state, it is able to parse a subset of C that does not yet include things like structs, unions, enums or function pointers
and compile it to something very close to proper x86_64 assembly. As this project is more of a proof of concept
and I don't really have the time to work on it, it will likely never be finished.
