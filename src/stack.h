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


#pragma once

#include "exception.h"

template <typename T>
class StackItem {
    public:
        T data;
        StackItem* next;
        StackItem* prev;
};

template <typename T>
class StackIter {
    private:
        StackItem<T>* current_item;
    public:
        StackIter(StackItem<T>* top) {
            this->current_item = top;
        }

        T& next() {
            if (current_item == nullptr)
                throw Exception("`StackIter::next()` called on empty stack");
            
            T& data = current_item->data;
            
            if (current_item->prev == nullptr)
                current_item = nullptr;
            else
                current_item = current_item->prev;
            return data;
        }

        bool has_next() {
            return current_item != nullptr;
        }
};

template <typename T>
class Stack {
    private:
        std::string identifier;
        StackItem<T>* top;
        u32 _size;

    public:
        Stack(std::string identifier);
        ~Stack();

        void push(T item);
        T pop();
        T& peek();
        u32 size();
        bool empty();

        StackIter<T> iter();
};

template <typename T>
Stack<T>::Stack(std::string identifier) {
    this->identifier = identifier;
    top = nullptr;
    _size = 0;
}

template <typename T>
Stack<T>::~Stack() {
    while (!empty()) {
        pop();
    }
}

template<typename T>
u32 Stack<T>::size() {
    return _size;
}

template <typename T>
void Stack<T>::push(T item) {
    StackItem<T>* insertion = new StackItem<T>{ item, nullptr, top };
    if (_size > 0)
        top->next = insertion;
    top = insertion;
    _size++;
}

template<typename T>
struct is_pointer { static const bool value = false; };

template<typename T>
struct is_pointer<T*> { static const bool value = true; };

template <typename T>
T Stack<T>::pop() {

    if (empty()) throw Exception("Tried to pop from empty stack \""+identifier+"\".");
    _size--;
    T ret = top->data;
    if (_size > 0) {
        top = top->prev;
        if (is_pointer<T>::value)
            delete top->next;
        top->next = nullptr;
    } else {
        if (is_pointer<T>::value)
            delete top;
        top = nullptr;
    }
    
    return ret;
}

template <typename T>
T& Stack<T>::peek() {
    if (empty()) throw Exception("Tried to peek from empty stack \""+identifier+"\".");  
    return top->data;
}

template <typename T>
bool Stack<T>::empty() {
    return _size < 1;
}

template <typename T>
StackIter<T> Stack<T>::iter() {
    return StackIter<T>(top);
}
