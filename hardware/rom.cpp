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

namespace ryu::hardware {

    rom::rom(
            int id,
            const std::string& name,
            uint8_t* buffer,
            size_t size,
            uint32_t address) : integrated_circuit(id, name),
                                _size(size),
                                _buffer(buffer),
                                _address(address) {
    }

    rom::~rom() {
        delete _buffer;
        _buffer = nullptr;
    }

    size_t rom::size() const {
        return _size;
    }

    uint32_t rom::address() const {
        return _address;
    }

    bool rom::write_latch() const {
        return _write_latch;
    }

    void rom::write_latch(bool enabled) {
        _write_latch = enabled;
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