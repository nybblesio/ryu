//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "rom.h"

RTTR_REGISTRATION {
    rttr::registration::class_<ryu::hardware::rom>("ryu::hardware::rom") (
        rttr::metadata(ryu::hardware::meta_data_key::type_id, ryu::hardware::rom_id),
        rttr::metadata(ryu::hardware::meta_data_key::type_name, "Pseudo EPROM IC")
    )
    .constructor<>(rttr::registration::public_access);
}

namespace ryu::hardware {

    void rom::init() {
    }

    rom::rom() : integrated_circuit("rom-ic") {
    }

    rom::~rom() {
        delete _buffer;
        _buffer = nullptr;
    }

    void rom::zero() {
        if (_write_latch)
            std::memset(_buffer, 0, address_space());
    }

    void rom::reallocate() {
        delete _buffer;
        _buffer = new uint8_t[address_space()];
    }

    void rom::fill(uint8_t value) {
        if (_write_latch)
            std::memset(_buffer, value, address_space());
    }

    bool rom::write_latch() const {
        return _write_latch;
    }

    void rom::write_latch(bool enabled) {
        _write_latch = enabled;
    }

    void rom::on_address_space_changed() {
        reallocate();
    }

    uint8_t rom::read_byte(uint32_t address) const {
        return _buffer[address];
    }

    void rom::write_byte(uint32_t address, uint8_t value) {
        if (!_write_latch)
            return;
        _buffer[address] = value;
    }

}