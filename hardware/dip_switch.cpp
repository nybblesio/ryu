//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "dip_switch.h"

RTTR_REGISTRATION {
    rttr::registration::class_<ryu::hardware::dip_switch>("ryu::hardware::dip_switch") (
            rttr::metadata(ryu::hardware::meta_data_key::type_id, ryu::hardware::dip_switch_id),
            rttr::metadata(ryu::hardware::meta_data_key::type_name, "Dip Switch"),
            rttr::metadata(ryu::hardware::meta_data_key::type_category, ryu::hardware::category_memory))
        .constructor<>(rttr::registration::public_access) (
                rttr::policy::ctor::as_raw_ptr
        );
}

namespace ryu::hardware {

    void dip_switch::init() {
    }

    dip_switch::dip_switch() : integrated_circuit("dip-switch-ic") {
    }

    void dip_switch::zero() {
        _switches.reset();
    }

    void dip_switch::on_initialize() {
    }

    void dip_switch::fill(uint8_t value) {
        for (uint32_t i = 0; i < address_space(); i++)
            write_byte(i, value);
    }

    void dip_switch::on_address_space_changed() {
        assert(address_space() <= 4);
        zero();
    }

    access_type_flags dip_switch::access_type() const {
        return access_types::writable | access_types::readable;
    }

    uint8_t dip_switch::read_byte(uint32_t address) const {
        uint8_t bits = 0;
        for (size_t i = 0; i < 8; i++)
            bits |= _switches[(address * 8) + i];
        return bits;
    }

    void dip_switch::write_byte(uint32_t address, uint8_t value) {
        uint8_t mask = 0b00000001;
        for (size_t i = 0; i <= 8; i++) {
            _switches[(address * 8) + i] = (value & mask) != 0;
            mask <<= 1;
        }
    }

    uint16_t dip_switch::read_word(
            uint32_t address,
            integrated_circuit::endianness::types endianess) const {
        return 0;
    }

    uint32_t dip_switch::read_dword(
            uint32_t address,
            integrated_circuit::endianness::types endianess) const {
        return 0;
    }

    std::vector<uint8_t> dip_switch::write_word(
            uint32_t address,
            uint16_t value,
            integrated_circuit::endianness::types endianess) {
        return {};
    }

    std::vector<uint8_t> dip_switch::write_dword(
            uint32_t address,
            uint32_t value,
            integrated_circuit::endianness::types endianess) {
        return {};
    }

}