//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <iostream>
#include "selection.h"

namespace ryu::core {

    void selection::clear() {
        _end = {0, 0, true};
        _start = {0, 0, true};
    }

    void selection::normalize() {
        if (_start.row > _end.row) {
            auto temp = _start.row;
            _start.row = _end.row;
            _end.row = temp;
        }
    }

    bool selection::valid() const {
        return !start().empty && !end().empty;
    }

    selection_point_t selection::end() const {
        return _end;
    }

    selection_point_t selection::start() const {
        return _start;
    }

    void selection::end(uint32_t row, uint16_t col) {
        _end = {row, col, false};
    }

    void selection::start(uint32_t row, uint16_t col) {
        _start = {row, col, false};
    }

    bool selection::selected(uint32_t row, uint16_t column) {
        auto e = end();
        auto s = start();
        auto selected = row >= s.row && row <= e.row;
        return selected;
    }

}