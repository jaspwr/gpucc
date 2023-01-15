#pragma once

#include <vector>

#include "types.h"

template <typename T>
class ExtendableBuffer {
    private:
        u32 size;
        u32 chunk_size;
        std::vector<T*> data;
    public:
        ExtendableBuffer(u32 chunk_size);
        ~ExtendableBuffer();

        void append(T byte);
        void add(T* data, u32 size);
        T* flattern();
        u32 get_size();

        T& operator[](u32 index);
};

template <typename T>
ExtendableBuffer<T>::ExtendableBuffer(u32 chunk_size) {
    this->chunk_size = chunk_size;
    this->size = 0;
    this->data = { new T[chunk_size] };
}

template <typename T>
ExtendableBuffer<T>::~ExtendableBuffer() {
    for (auto chunk : data) {
        delete[] chunk;
    }
}

template <typename T>
void ExtendableBuffer<T>::append(T byte) {
    u32 chunk_index = size / chunk_size;
    u32 chunk_offset = size % chunk_size;
    if (chunk_offset == 0) {
        data.push_back(new T[chunk_size]);
    }
    data[chunk_index][chunk_offset] = byte;
    size++;
}

template <typename T>
void ExtendableBuffer<T>::add(T* data, u32 size) {
    T* data_T = (T*)data;
    for (u32 i = 0; i < size; i++) {
        append(data_T[i]);
    }   
}

template <typename T>
T* ExtendableBuffer<T>::flattern() {
    T* ret = new T[size];
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

template <typename T>
u32 ExtendableBuffer<T>::get_size() {
    return size;
}

template <typename T>
T& ExtendableBuffer<T>::operator[](u32 index) {
    return data[index / chunk_size][index % chunk_size];
}