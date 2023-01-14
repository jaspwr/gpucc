#pragma once

#include <vector>

#include "types.h"

class ExtendableBuffer {
    private:
        u32 size;
        u32 chunk_size;
        std::vector<u8*> data;
    public:
        ExtendableBuffer(u32 chunk_size);
        ~ExtendableBuffer();

        void append(u8 byte);
        void add(void* data, u32 size);
        u8* flattern();
        u32 get_size();
};