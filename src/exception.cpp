#include "exception.h"

Exception::Exception(const char* message) {
    this->message = std::string(message);
    type = ExceptionType::Misc;
}

Exception::Exception(std::string message) {
    this->message = message;
    type = ExceptionType::Misc;
}

Exception::Exception(ExceptionType type, const char* message) {
    this->message = std::string(message);
    this->type = type;
}

Exception::Exception(ExceptionType type, std::string message) {
    this->message = message;
    this->type = type;
}

void Exception::print() {
    // TODO: Make a nice class or function or something for adding terminal colours to strings.
    switch (type) {
    case ExceptionType::Misc:
        printf("\033[1;31mError\033[0m %s\n", message.c_str());
        break;
    case ExceptionType::File:
        printf("\033[1;31mFile error\033[0m %s\n", message.c_str());
        break;
    case ExceptionType::Yacc:
        printf("\033[1;31mYACC error\033[0m %s\n", message.c_str());
        break;
    case ExceptionType::Preprocessor:
        printf("\033[1;31mPreprocessor error\033[0m %s\n", message.c_str());
        break;
    case ExceptionType::Postprocessor:
        printf("\033[1;31mPostprocessor error\033[0m %s\n", message.c_str());
        break;
    }
    
}