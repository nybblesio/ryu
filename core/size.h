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

#include <cstdint>

namespace ryu::core {

    class size {
    public:
        size();

        size(uint32_t width, uint32_t height);

        uint32_t width() const;

        uint32_t height() const;

        void width(uint32_t value);

        inline bool empty() const {
            return _width == 0 && _height == 0;
        }

        void height(uint32_t value);

        void dimensions(uint32_t width, uint32_t height);

    private:
        uint32_t _width;
        uint32_t _height;
    };

};

