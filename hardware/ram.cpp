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

RTTR_REGISTRATION {
    rttr::registration::class_<ryu::hardware::ram>("ryu::hardware::ram") (
        rttr::metadata(ryu::hardware::meta_data_key::type_id, ryu::hardware::ram_id),
        rttr::metadata(ryu::hardware::meta_data_key::type_name, "RAM IC")
    )
    .constructor<>(rttr::registration::public_access)  (
            rttr::policy::ctor::as_raw_ptr
    );
}

namespace ryu::hardware {

    ram::ram() : integrated_circuit("ram-ic") {
    }

    ram::~ram() {
        delete _buffer;
        _buffer = nullptr;
    }

    void ram::init() {
    }

    void ram::reallocate() {
        delete _buffer;
        _buffer = new uint8_t[address_space()];
    }

    void ram::zero() {
        std::memset(_buffer, 0, address_space());
    }

    void ram::fill(uint8_t value) {
        std::memset(_buffer, value, address_space());
    }

    void ram::on_address_space_changed() {
        reallocate();
    }

    uint8_t ram::read_byte(uint32_t address) const {
        return _buffer[address];
    }

    void ram::write_byte(uint32_t address, uint8_t value) {
        _buffer[address] = value;
    }

}
