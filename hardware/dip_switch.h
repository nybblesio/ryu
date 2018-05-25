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
#include "integrated_circuit.h"

namespace ryu::hardware {

    class dip_switch : public hardware::integrated_circuit {
    public:
        static void init();

        dip_switch();

        void zero() override;

        void fill(uint8_t value) override;

        uint32_t type_id() const override {
            return ryu::hardware::dip_switch_id;
        }

        access_type_flags access_type() const override;

        uint8_t read_byte(uint32_t address) const override;

        uint16_t read_word(
                uint32_t address,
                integrated_circuit::endianness::types endianess) const override;

        uint32_t read_dword(
                uint32_t address,
                integrated_circuit::endianness::types endianess) const override;

        void write_byte(uint32_t address, uint8_t value) override;

        std::vector<uint8_t> write_word(
                uint32_t address,
                uint16_t value,
                integrated_circuit::endianness::types endianess) override;

        std::vector<uint8_t> write_dword(
                uint32_t address,
                uint32_t value,
                integrated_circuit::endianness::types endianess) override;

    protected:
        void on_initialize() override;

        void on_address_space_changed() override;

    RTTR_ENABLE(hardware::integrated_circuit)

    private:
        std::bitset<32> _switches;
    };

};

