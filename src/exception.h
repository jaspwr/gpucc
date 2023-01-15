#pragma once

#include "types.h"

#include <string>

enum class ExceptionType: u32 {
    Misc,
    File,
    Preprocessor,
    Postprocessor,
    Yacc,
};

class Exception {
private:
    std::string message;

public:
    ExceptionType type;

    Exception(const char* message);
    Exception(std::string message);
    Exception(ExceptionType type, const char* message);
    Exception(ExceptionType type, std::string message);

    void print();
};
