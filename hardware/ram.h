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

    class ram : public integrated_circuit,
                public memory {
    public:
        ram(
                int id,
                const std::string& name,
                size_t size,
                uint32_t address);

        ~ram() override;

        void zero() override;

        size_t size() const override;

        void fill(uint8_t value) override;

        uint32_t address_space() const override;

        uint8_t read_byte(uint32_t address) const override;

        void write_byte(uint32_t address, uint8_t value) override;

    private:
        size_t _size;
        uint8_t* _buffer = nullptr;
    };

};

