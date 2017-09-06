//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "memory_map_entry.h"

namespace ryu::hardware {

    memory_map_entry::memory_map_entry(
            uint32_t address,
            uint32_t size) : _size(size),
                             _address(address) {
    }


}