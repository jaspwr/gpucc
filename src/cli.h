#pragma once

#include "job.h"

struct TerminalSize {
    u32 rows;
    u32 cols;
};

Job parse_args(int argc, char** argv);
TerminalSize get_terminal_size();