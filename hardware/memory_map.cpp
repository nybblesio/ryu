//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "memory_map.h"

namespace ryu::hardware {

    memory_map::memory_map(
            int id,
            const std::string& name,
            uint32_t address_space) : integrated_circuit(id, name) {
    }

    void memory_map::zero() {
    }

    void memory_map::fill(uint8_t value) {
    }

    uint32_t memory_map::address_space() const {
        return _address_space;
    }

    uint8_t memory_map::read_byte(uint32_t address) const {
        return 0;
    }

    void memory_map::write_byte(uint32_t address, uint8_t value) {
    }

}