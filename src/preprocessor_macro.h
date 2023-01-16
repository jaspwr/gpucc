#pragma once

#include <string>

#include "types.h"
#include "extendable_buffer.h"
#include "string_template.h"

class PreprocessorMacro {
    public:
        virtual void exectue(ExtendableBuffer<u8>* buffer, u32& i, const char* source) {};
};

class ObjectLikeMacro : public PreprocessorMacro {
    private:
        std::string value;
    public:
        ObjectLikeMacro(std::string value);
        void exectue(ExtendableBuffer<u8>* buffer, u32& i, const char* source) override;
};

class FunctionLikeMacro : public PreprocessorMacro {
    private:
        StringTemplate template_value;

    public:
        FunctionLikeMacro(std::string value);
        void exectue(ExtendableBuffer<u8>* buffer, u32& i, const char* source) override;
};
