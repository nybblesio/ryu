//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "padding.h"

namespace ryu::core {

    padding::padding(
            int32_t left,
            int32_t right,
            int32_t top,
            int32_t bottom) : _left(left),
                              _right(right),
                              _top(top),
                              _bottom(bottom) {
    }

    int32_t padding::left() const {
        return _left;
    }

    void padding::left(int32_t value) {
        _left = value;
    }

    int32_t padding::right() const {
        return _right;
    }

    void padding::right(int32_t value) {
        _right = value;
    }

    int32_t padding::top() const {
        return _top;
    }

    void padding::top(int32_t value) {
        _top = value;
    }

    int32_t padding::bottom() const {
        return _bottom;
    }

    void padding::bottom(int32_t value) {
        _bottom = value;
    }

    int32_t padding::horizontal() const {
        return _left + _right;
    }

    int32_t padding::vertical() const {
        return _top + _bottom;
    }

}