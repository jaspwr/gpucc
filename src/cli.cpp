#include "cli.h"
#include "exception.h"

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>

std::string get_next_arg(int& argc, char**& argv) {
    argc--;
    argv++;
    if (argc == 1) { throw Exception("Expected argument."); }
    return std::string(*argv);
}

void consume_arg(int& argc, char**& argv, Job& job) {
    if (strcmp(*argv, "-o") == 0) {
        job.output_file = get_next_arg(argc, argv);
    } else if (strcmp(*argv, "-y") == 0) {
        job.yacc = get_next_arg(argc, argv);
    } else if (strcmp(*argv, "--dbg") == 0) {
        job.dbg = true;
    } else if (strcmp(*argv, "-emit") == 0) {
        std::string emit = get_next_arg(argc, argv);
        if (emit == "mew-ir") {
            job.emit = Emit::mew_ir;
        } else if (emit == "llvm-ir") {
            job.emit = Emit::llvm_ir;
        } else if (emit == "llvm-bc") {
            job.emit = Emit::llvm_bc;
        } else {
            throw "Invalid emit type.";
        }
    } else {
        job.source_files.push_back(*argv);
    }
    argc--;
    argv++;
};

Job parse_args(int argc, char** argv) {
    Job job;
    argv++;
    while (argc > 1) {
        consume_arg(argc, argv, job);
    }
    return job;
}

TerminalSize get_terminal_size() {
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    return TerminalSize { w.ws_row, w.ws_col };
}