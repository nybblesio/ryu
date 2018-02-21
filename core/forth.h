//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#pragma once

#include <stack>
#include <string>
#include <vector>
#include <functional>
#include "result.h"

namespace ryu::core {

    class forth_stack {
    public:
        forth_stack(uint8_t* buffer, uint32_t top) : _start(top - 4),
                                                     _buffer(buffer) {
        }

        void add();

        void sub();

        void mul();

        void div();

        void mod();

        void inc();

        void dec();

        void dup();

        void rot();

        void inc4();

        void dec4();

        void swap();

        void swap2();

        void clear();

        uint32_t pop();

        uint32_t peek();

        uint32_t over();

        void reverse_rot();

        void dup_if_ne_zero();

        inline bool empty() {
            return _start == *top();
        }

        inline size_t depth() {
            return (_start - *top()) / 4;
        }

        void push(uint32_t value);

    protected:
        void grow();

        void shrink();

        uint32_t* top();

        uint32_t* from_top(uint32_t slot);

    private:
        uint32_t _start;
        uint8_t* _buffer = nullptr;
    };

    using native_codeword_callable = std::function<void ()>;
    using native_codeword_list = std::vector<native_codeword_callable>;

    class forth {
    public:
        const uint32_t heap_size  = 1024 * 256;
        const uint32_t stack_size = 1024 * 32;
        const uint32_t data_stack_start = heap_size;
        const uint32_t var_s0 = heap_size;

        const uint32_t return_stack_start = heap_size - stack_size;
        const uint32_t var_r0 = heap_size - stack_size;

        const uint32_t var_here   = return_stack_start - stack_size - 4;
        const uint32_t var_latest = var_here - 4;
        const uint32_t var_base   = var_latest - 4;

        const uint32_t native_dup  = 0;
        const uint32_t native_rot  = 1;
        const uint32_t native_swap = 2;
        const uint32_t native_drop = 3;

        forth();

        virtual ~forth();

        std::istream* in();

        std::ostream* out();

        void in(std::istream* in);

        void out(std::ostream* out);

        bool initialize(core::result& result);

    protected:
        uint32_t* s0() {
            return reinterpret_cast<uint32_t*>(_heap + var_s0);
        }

        uint32_t* r0() {
            return reinterpret_cast<uint32_t*>(_heap + var_r0);
        }

        uint32_t* here() {
            return reinterpret_cast<uint32_t*>(_heap + var_here);
        }

        uint32_t* latest() {
            return reinterpret_cast<uint32_t*>(_heap + var_latest);
        }

    private:
        void dup();

        void rot();

        void drop();

        void swap();

    private:
        uint8_t* _heap;
        forth_stack _data;
        forth_stack _return;
        std::istream* _in = nullptr;
        std::ostream* _out = nullptr;
        native_codeword_list _native_codewords {};
    };

};

