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

namespace ryu::hardware {

    class memory_map : public hardware::integrated_circuit {
    public:
        memory_map(
                int id,
                const std::string& name,
                uint32_t address_space);

        void zero() override;

        void fill(uint8_t value) override;

        uint32_t address_space() const override;

        uint8_t read_byte(uint32_t address) const override;

        void write_byte(uint32_t address, uint8_t value) override;

    private:
        uint32_t _address_space = 0;
    };

};

