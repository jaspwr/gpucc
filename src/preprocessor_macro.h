// Copyright (C) 2023 Jasper Parker <j@sperp.dev>
//
// This file is part of gpucc.
//
// gpucc is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// gpucc is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with gpucc. If not, see <https://www.gnu.org/licenses/>.


#pragma once

#include <string>

#include "types.h"
#include "extendable_buffer.h"
#include "string_template.h"

class PreprocessorMacro {
    public:
        virtual void execute(ExtendableBuffer<u8>* buffer, u32& i, const char* source) {};
};

class ObjectLikeMacro : public PreprocessorMacro {
    private:
        std::string value;
    public:
        ObjectLikeMacro(std::string value);
        void execute(ExtendableBuffer<u8>* buffer, u32& i, const char* source) override;
};

class FunctionLikeMacro : public PreprocessorMacro {
    private:
        StringTemplate template_value;

    public:
        FunctionLikeMacro(std::string value);
        void execute(ExtendableBuffer<u8>* buffer, u32& i, const char* source) override;
};
