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

#include <utility>

namespace ryu::core {

    struct selection_point_t {
        uint32_t row;
        uint16_t column;
        bool empty = true;
    };

    class selection {
    public:
        selection() = default;

        void clear();

        void normalize();

        bool valid() const;

        selection_point_t end() const;

        selection_point_t start() const;

        void end(uint32_t row, uint16_t col);

        void start(uint32_t row, uint16_t col);

        bool selected(uint32_t row, uint16_t column);

    private:
        selection_point_t _end = {0, 0};
        selection_point_t _start = {0, 0};
    };

};

