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

        uint8_t read_byte(uint32_t address) const override;

        void release(hardware::integrated_circuit* component);

        void write_byte(uint32_t address, uint8_t value) override;

        integrated_circuit* circuit_at_address(uint32_t address) const;

        RTTR_ENABLE(hardware::integrated_circuit)

    private:
        ic_interval_list _components;
    };

};

