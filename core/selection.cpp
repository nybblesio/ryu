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
        _end = {-1, -1};
        _start = {-1, -1};
    }

    bool selection::valid() const {
        auto empty = std::pair<int, int>{-1, -1};
        return start() != empty && end() != empty;
    }

    void selection::end(int row, int col) {
        _end = {row, col};
    }

    void selection::start(int row, int col) {
        _start = {row, col};
    }

    std::pair<int, int> selection::end() const {
        return _end;
    }

    std::pair<int, int> selection::start() const {
        return _start;
    }

    bool selection::selected(int row, int column) {
        auto e = end();
        auto s = start();
        auto selected = row >= s.first && row <= e.first;
        return selected;
    }

}