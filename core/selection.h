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

    class selection {
    public:
        selection() = default;

        void clear();

        void normalize();

        bool valid() const;

        void end(int row, int col);

        void start(int row, int col);

        std::pair<int, int> end() const;

        std::pair<int, int> start() const;

        bool selected(int row, int column);

    private:
        std::pair<int, int> _end = {-1, -1};
        std::pair<int, int> _start = {-1, -1};
    };

};

