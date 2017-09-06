//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "palette_entry.h"

namespace ryu::core {

    palette_entry::palette_entry(
            uint8_t red,
            uint8_t green,
            uint8_t blue,
            uint8_t alpha) : _red(red),
                             _green(green),
                             _blue(blue),
                             _alpha(alpha) {
    }

    uint8_t palette_entry::red() const {
        return _red;
    }

    uint8_t palette_entry::green() const {
        return _green;
    }

    uint8_t palette_entry::blue() const {
        return _blue;
    }

    uint8_t palette_entry::alpha() const {
        return _alpha;
    }

    void palette_entry::red(uint8_t value) {
        _red = value;
    }

    void palette_entry::green(uint8_t value) {
        _green = value;
    }

    void palette_entry::blue(uint8_t value) {
        _blue = value;
    }

    void palette_entry::alpha(uint8_t value) {
        _alpha = value;
    }

    palette_entry palette_entry::fade(uint8_t factor) {
        return {_red /= factor, _green /= factor, _blue /= factor, _alpha};
    }

}