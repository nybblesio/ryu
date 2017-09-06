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

    class rom : public integrated_circuit,
                public memory {
    public:
        rom(
                int id,
                const std::string& name,
                uint8_t* buffer,
                size_t size,
                uint32_t address);

        ~rom() override;

        bool write_latch() const;

        uint32_t address() const;

        size_t size() const override;

        void write_latch(bool enabled);

        uint8_t read_byte(uint32_t address) const override;

        void write_byte(uint32_t address, uint8_t value) override;

    private:
        size_t _size;
        uint32_t _address;
        bool _write_latch = false;
        uint8_t* _buffer = nullptr;
    };

};

