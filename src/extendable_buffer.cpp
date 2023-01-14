#include "extendable_buffer.h"

ExtendableBuffer::ExtendableBuffer(u32 chunk_size) {
    this->chunk_size = chunk_size;
    this->size = 0;
    this->data = { new u8[chunk_size] };
}

ExtendableBuffer::~ExtendableBuffer() {
    for (auto chunk : data) {
        delete[] chunk;
    }
}

void ExtendableBuffer::append(u8 byte) {
    u32 chunk_index = size / chunk_size;
    u32 chunk_offset = size % chunk_size;
    if (chunk_offset == 0) {
        data.push_back(new u8[chunk_size]);
    }
    data[chunk_index][chunk_offset] = byte;
    size++;
}

void ExtendableBuffer::add(void* data, u32 size) {
    u8* data_u8 = (u8*)data;
    for (u32 i = 0; i < size; i++) {
        append(data_u8[i]);
    }   
}

u8* ExtendableBuffer::flattern() {
    u8* ret = new u8[size];
    u32 index = 0;
    for (auto chunk : data) {
        for (u32 i = 0; i < chunk_size; i++) {
            ret[index] = chunk[i];
            index++;
            if (index == size) {
                return ret;
            }
        }
    }
    return ret;
}

u32 ExtendableBuffer::get_size() {
    return size;
}
