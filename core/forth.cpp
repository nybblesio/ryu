//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "forth.h"

namespace ryu::core {

    forth::forth() : _heap(new uint8_t[heap_size]),
                     _data(_heap, data_stack_start),
                     _return(_heap, return_stack_start) {
    }

    forth::~forth() {
        delete[] _heap;
        _heap = nullptr;
    }

    void forth::dup() {
        _data.dup();
    }

    void forth::rot() {
        _data.rot();
    }

    void forth::drop() {
        _data.pop();
    }

    void forth::swap() {
        _data.swap();
    }

    std::istream* forth::in() {
        return _in;
    }

    std::ostream* forth::out() {
        return _out;
    }

    void forth::in(std::istream* in) {
        _in = in;
    }

    void forth::out(std::ostream* out) {
        _out = out;
    }

    bool forth::initialize(core::result& result) {
        _native_codewords[native_dup]  = [&]() { dup(); };
        _native_codewords[native_rot]  = [&]() { rot(); };
        _native_codewords[native_swap] = [&]() { swap(); };
        _native_codewords[native_drop] = [&]() { drop(); };
        return !result.is_failed();
    }

    ///////////////////////////////////////////////////////////////////////////

    void forth_stack::inc() {
        auto top_ptr = from_top(0);
        (*top_ptr)++;
    }

    void forth_stack::dec() {
        auto top_ptr = from_top(0);
        (*top_ptr)--;
    }

    void forth_stack::dup() {
        push(*from_top(0));
    }

    void forth_stack::rot() {
//        auto slot0_ptr = from_top(0);
//        auto slot1_ptr = from_top(1);
//        auto slot2_ptr = from_top(2);
    }

    void forth_stack::add() {
        auto lhs = pop();
        auto rhs = from_top(0);
        *rhs = lhs + *rhs;
    }

    void forth_stack::sub() {
        auto lhs = pop();
        auto rhs = from_top(0);
        *rhs = lhs - *rhs;
    }

    void forth_stack::mul() {
        auto lhs = pop();
        auto rhs = from_top(0);
        *rhs = lhs * *rhs;
    }

    void forth_stack::div() {
        auto lhs = pop();
        auto rhs = from_top(0);
        *rhs = lhs / *rhs;
    }

    void forth_stack::mod() {
        auto lhs = pop();
        auto rhs = from_top(0);
        *rhs = lhs % *rhs;
    }

    void forth_stack::inc4() {
        auto top_ptr = from_top(0);
        (*top_ptr) += 4;
    }

    void forth_stack::dec4() {
        auto top_ptr = from_top(0);
        (*top_ptr) -= 4;
    }

    void forth_stack::grow() {
        *top() -= 4;
    }

    void forth_stack::swap() {
        auto slot0_ptr = from_top(0);
        auto slot1_ptr = from_top(1);
        auto temp = *slot0_ptr;
        *slot0_ptr = *slot1_ptr;
        *slot1_ptr = temp;
    }

    void forth_stack::swap2() {
//        auto slot0_ptr = from_top(0);
//        auto slot1_ptr = from_top(1);
//        auto slot2_ptr = from_top(2);
//        auto slot3_ptr = from_top(3);
//        auto temp = *slot0_ptr;
//        *slot0_ptr = *slot1_ptr;
//        *slot1_ptr = temp;
    }

    void forth_stack::clear() {
        *top() = _start;
    }

    void forth_stack::shrink() {
        *top() += 4;
    }

    uint32_t forth_stack::pop() {
        uint32_t value = *from_top(0);
        shrink();
        return value;
    }

    uint32_t forth_stack::peek() {
        return *from_top(0);
    }

    uint32_t forth_stack::over() {
        return *from_top(1);
    }

    uint32_t* forth_stack::top() {
        return reinterpret_cast<uint32_t*>(_buffer + (_start + 4));
    }

    void forth_stack::reverse_rot() {
//        auto slot0_ptr = from_top(0);
//        auto slot1_ptr = from_top(1);
//        auto slot2_ptr = from_top(2);
    }

    void forth_stack::dup_if_ne_zero() {
        auto slot0_ptr = from_top(0);
        auto value = *slot0_ptr;
        if (value != 0)
            push(value);
    }

    void forth_stack::push(uint32_t value) {
        grow();
        auto slot0 = *top();
        uint8_t* byte_ptr = reinterpret_cast<uint8_t*>(&value);
        _buffer[slot0 + 0] = *(byte_ptr + 0);
        _buffer[slot0 + 1] = *(byte_ptr + 1);
        _buffer[slot0 + 2] = *(byte_ptr + 2);
        _buffer[slot0 + 3] = *(byte_ptr + 3);
    }

    uint32_t* forth_stack::from_top(uint32_t slot) {
        return reinterpret_cast<uint32_t*>(_buffer + (*top() + (slot * 4)));
    }

}