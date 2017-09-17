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

namespace ryu {

    inline uint8_t set_lower_nybble(uint8_t original, uint8_t value) {
        uint8_t res = original;
        res &= 0xF0;
        res |= (value & 0x0F);
        return res;
    }

    inline uint8_t set_upper_nybble(uint8_t original, uint8_t value) {
        uint8_t res = original;
        res &= 0x0F;
        res |= ((value << 4) & 0xF0);
        return res;
    }

}