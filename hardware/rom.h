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

    class rom : public hardware::integrated_circuit {
    public:
        static void init();

        rom();

        ~rom() override;

        void zero() override;

        bool write_latch() const;

        void write_latch(bool enabled);

        void fill(uint8_t value) override;

        uint8_t read_byte(uint32_t address) const override;

        void write_byte(uint32_t address, uint8_t value) override;

    protected:
        void on_address_space_changed() override;

        RTTR_ENABLE(hardware::integrated_circuit)

    private:
        void reallocate();

        bool _write_latch = false;
        uint8_t* _buffer = nullptr;
    };

};

