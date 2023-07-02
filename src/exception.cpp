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
    case ExceptionType::InstructionSelectionSchema:
        printf("\033[1;31mInstruction selection schema error\033[0m %s\n", message.c_str());
        break;
    default:
        printf("\033[1;31mError\033[0m %s\n", message.c_str());
        printf("NOTE: This is an unknown exception type. Add a case for %d in the switch in `Exception::print` in exception.cpp.\n", (u32)type);
        break;
    }

}
