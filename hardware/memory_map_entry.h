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

namespace ryu::hardware {

    class memory_map_entry {
    public:
        memory_map_entry(uint32_t address, uint32_t size);

    private:
        uint32_t _size = 0;
        uint32_t _address = 0;
    };

};

