//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "ram.h"

namespace ryu::hardware {

    ram::ram(
            int id,
            const std::string& name,
            size_t size,
            uint32_t address) : integrated_circuit(id, name),
                                _size(size) {
        _buffer = new uint8_t[size];
        this->address(address);
    }

    ram::~ram() {
        delete _buffer;
        _buffer = nullptr;
    }

    void ram::zero() {
        std::memset(_buffer, 0, _size);
    }

    size_t ram::size() const {
        return _size;
    }

    void ram::fill(uint8_t value) {
        std::memset(_buffer, value, _size);
    }

    uint32_t ram::address_space() const {
        // max 256MB of RAM per instance
        return 256 * (1024 * 1024);
    }

    uint8_t ram::read_byte(uint32_t address) const {
        return _buffer[address];
    }

    void ram::write_byte(uint32_t address, uint8_t value) {
        _buffer[address] = value;
    }

}
