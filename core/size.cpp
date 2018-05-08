//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "size.h"

namespace ryu::core {

    size::size() : _width(0),
                   _height(0) {
    }

    size::size(uint32_t width, uint32_t height) : _width(width),
                                                  _height(height) {
    }

    uint32_t size::width() const {
        return _width;
    }

    uint32_t size::height() const {
        return _height;
    }

    void size::width(uint32_t value) {
        _width = value;
    }

    void size::height(uint32_t value) {
        _height = value;
    }

    void size::dimensions(uint32_t width, uint32_t height) {
        _width = width;
        _height = height;
    }

}