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

#include "integrated_circuit.h"
#include "memory.h"

namespace ryu::hardware {

    class ram : public hardware::integrated_circuit {
    public:
        static void init();

        ram();

        ~ram() override;

        void write_byte(
                uint32_t address,
                uint8_t value) override;

        ryu::core::byte_list write_word(
                uint32_t address,
                uint16_t value,
                integrated_circuit::endianness::types endianess) override;

        ryu::core::byte_list write_dword(
                uint32_t address,
                uint32_t value,
                integrated_circuit::endianness::types endianess) override;

        uint16_t read_word(
                uint32_t address,
                integrated_circuit::endianness::types endianess) const override;

        uint32_t read_dword(
                uint32_t address,
                integrated_circuit::endianness::types endianess) const override;

        void zero() override;

        void fill(uint8_t value) override;

        uint32_t type_id() const override {
            return ryu::hardware::ram_id;
        }

        access_type_flags access_type() const override;

        uint8_t read_byte(uint32_t address) const override;

    protected:
        void on_initialize() override;

        void on_address_space_changed() override;

    private:
        void reallocate();

        RTTR_ENABLE(hardware::integrated_circuit)

    private:
        uint8_t* _buffer = nullptr;
    };

};

