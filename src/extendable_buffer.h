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
        std::vector<T> rollback(u32 count);
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

template <typename T>
std::vector<T> ExtendableBuffer<T>::rollback(u32 count) {
    size -= count;
    std::vector<T> ret = std::vector<T>();
    for (u32 i = 0; i < count; i++) {
        ret.push_back((*this)[size + i]);
    }
    return ret;
}
