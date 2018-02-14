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

#include <rttr/type>
#include <common/IntervalTree.h>
#include "integrated_circuit.h"

namespace ryu::hardware {

    class memory_mapper : public hardware::integrated_circuit {
    public:
        static void init();

        memory_mapper();

        void clear();

        void reserve(
            uint32_t address,
            hardware::integrated_circuit* component);

        void zero() override;

        void fill(uint8_t value) override;

        const ic_interval_list& components() const;

        access_type_flags access_type() const override;

        uint8_t read_byte(uint32_t address) const override;

        void release(hardware::integrated_circuit* component);

        void write_byte(uint32_t address, uint8_t value) override;

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

    protected:
        struct component_address_space_t {
            uint32_t start {};
            uint32_t end {};
            hardware::integrated_circuit* ic = nullptr;
        };

        memory_mapper::component_address_space_t circuit_at_address(
            integrated_circuit::access_types access_type,
            uint32_t address) const;

        RTTR_ENABLE(hardware::integrated_circuit)

    private:
        ic_interval_list _components;
    };

};

