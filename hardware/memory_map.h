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

    class memory_map : public hardware::integrated_circuit {
    public:
        memory_map(
                int id,
                const std::string& name,
                uint32_t address_space);

        void clear();

        void reserve(
                uint32_t address,
                hardware::integrated_circuit* component);

        void zero() override;

        void fill(uint8_t value) override;

        std::string type() const override {
            return "memory mapper";
        }

        uint32_t address_space() const override;

        uint8_t read_byte(uint32_t address) const override;

        void write_byte(uint32_t address, uint8_t value) override;

        ic_interval_list components_at_address(uint32_t address) const;

    private:
        RTTR_ENABLE(hardware::integrated_circuit)

        uint32_t _address_space = 0;
        ic_interval_list _components;
    };

};

