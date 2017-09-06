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

        void zero();

        uint32_t address() const;

        void fill(uint8_t value);

        size_t size() const override;

        uint8_t read_byte(uint32_t address) const override;

        void write_byte(uint32_t address, uint8_t value) override;

    private:
        size_t _size;
        uint32_t _address;
        uint8_t* _buffer = nullptr;
    };

};

