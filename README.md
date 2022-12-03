# meowcc [unfinished]

```
 /\_/\
( o.o )
 > C <
 ```
A prototype partially GPU based compiler focused on reducing compile times for larger jobs. This project currently compiles C to LLVM IR using OpenGL compute shaders but aims to eventually compile other languages such as C++, replace more parts of the compiler with parallel GPU based processing and possibly offer options to compiler with an OpenCL version if it may offer any compatability or performance advantages to compute shaders.

Major issues:
- The way this is set up currently requires a graphics server such as X11. This may have to be rectified by restarting the project in OpenCL.
- The shaders are constantly converting between floats and ints due to disallowed input types.

## Usage

```
meow [c file]
```

Optional flags:
- `-v` Verbose: More output
- `-emit-llvm` Emit LLVM: Output raw LLVM IR as ll file
