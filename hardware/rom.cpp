//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <common/bytes.h>
#include "rom.h"

RTTR_REGISTRATION {
    rttr::registration::class_<ryu::hardware::rom>("ryu::hardware::rom") (
        rttr::metadata(ryu::hardware::meta_data_key::type_id, ryu::hardware::rom_id),
        rttr::metadata(ryu::hardware::meta_data_key::type_name, "Pseudo EPROM IC")
    )
    .constructor<>(rttr::registration::public_access) (
            rttr::policy::ctor::as_raw_ptr
    );
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
        if (write_latch())
            std::memset(_buffer, 0, address_space());
    }

    void rom::reallocate() {
        clear_memory_map();

        add_memory_map_entry(0, address_space(), "ROM", "Read-only memory block.");

        delete _buffer;
        _buffer = new uint8_t[address_space()];

        // XXX: this is for debug purposes only, should remove or
        //        make configurable
        write_latch(true);
        fill(0xfe);
        write_latch(false);
    }

    uint16_t rom::read_word(
            uint32_t address,
            integrated_circuit::endianness::types endianess) const {
        auto value = *(reinterpret_cast<uint16_t*>(_buffer + address));

        if (is_platform_little_endian()
        &&  endianess == integrated_circuit::endianness::types::big) {
            return endian_swap_word(value);
        }

        return value;
    }

    uint32_t rom::read_dword(
            uint32_t address,
            integrated_circuit::endianness::types endianess) const {
        auto value = *(reinterpret_cast<uint32_t*>(_buffer + address));

        if (is_platform_little_endian()
        &&  endianess == integrated_circuit::endianness::types::big) {
            return endian_swap_dword(value);
        }

        return value;
    }

    ryu::core::byte_list rom::write_word(
            uint32_t address,
            uint16_t value,
            integrated_circuit::endianness::types endianess) {
        ryu::core::byte_list data {};

        if (is_platform_little_endian()
        &&  endianess == integrated_circuit::endianness::types::big) {
            value = endian_swap_word(value);
        }
        auto byte_ptr = reinterpret_cast<uint8_t*>(&value);
        _buffer[address]     = *byte_ptr;
        _buffer[address + 1] = *(byte_ptr + 1);

        data.push_back(*byte_ptr);
        data.push_back(*(byte_ptr + 1));

        return data;
    }

    ryu::core::byte_list rom::write_dword(
            uint32_t address,
            uint32_t value,
            integrated_circuit::endianness::types endianess) {
        ryu::core::byte_list data {};

        if (is_platform_little_endian()
        &&  endianess == integrated_circuit::endianness::types::big) {
            value = endian_swap_dword(value);
        }
        auto byte_ptr = reinterpret_cast<uint8_t*>(&value);
        _buffer[address]     = *byte_ptr;
        _buffer[address + 1] = *(byte_ptr + 1);
        _buffer[address + 2] = *(byte_ptr + 2);
        _buffer[address + 3] = *(byte_ptr + 3);

        data.push_back(*byte_ptr);
        data.push_back(*(byte_ptr + 1));
        data.push_back(*(byte_ptr + 2));
        data.push_back(*(byte_ptr + 3));

        return data;
    }

    void rom::fill(uint8_t value) {
        if (write_latch())
            std::memset(_buffer, value, address_space());
    }

    void rom::on_address_space_changed() {
        reallocate();
    }

    access_type_flags rom::access_type() const {
        if (write_latch())
            return access_types::readable | access_types::writable;
        else
            return access_types::readable;
    }

    uint8_t rom::read_byte(uint32_t address) const {
        return _buffer[address];
    }

    void rom::write_byte(uint32_t address, uint8_t value) {
        if (!write_latch())
            return;
        _buffer[address] = value;
    }

}