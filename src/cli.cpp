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
