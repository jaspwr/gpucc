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
class Stack {
    public:
        Stack();
        ~Stack();

        void push(T item);
        T pop();
        T peek();
        u32 size();
        bool empty();

    private:
        StackItem<T>* top;
        u32 _size;
};

template <typename T>
Stack<T>::Stack() {
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

template <typename T>
T Stack<T>::pop() {

    if (empty()) throw Exception("Tried to pop from empty stack.");
    _size--;
    T ret = top->data;
    if (_size > 0) {
        top = top->prev;
        delete top->next;
        top->next = nullptr;
    } else {
        delete top;
        top = nullptr;
    }
    
    return ret;
}

template <typename T>
T Stack<T>::peek() {
    if (empty()) throw Exception("Tried to peek from empty stack.");  
    return top->data;
}

template <typename T>
bool Stack<T>::empty() {
    return _size < 1;
}