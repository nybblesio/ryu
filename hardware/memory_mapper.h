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

        access_type_flags access_type() const override;

        uint8_t read_byte(uint32_t address) const override;

        void release(hardware::integrated_circuit* component);

        void write_byte(uint32_t address, uint8_t value) override;

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

