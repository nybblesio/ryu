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

        void zero() override;

        void fill(uint8_t value) override;

        access_type_flags access_type() const override;

        uint8_t read_byte(uint32_t address) const override;

        void write_byte(uint32_t address, uint8_t value) override;

    protected:
        void on_address_space_changed() override;

    private:
        void reallocate();

        RTTR_ENABLE(hardware::integrated_circuit)

    private:
        uint8_t* _buffer = nullptr;
    };

};

