//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <hardware/integrated_circuit.h>
#include "opcode.h"

namespace ryu::hardware::mc6809 {

    class cpu : public hardware::integrated_circuit {
    public:
        static void init();

        cpu(int id, const std::string& name);

        uint32_t address_space() const override;

        RTTR_ENABLE(hardware::integrated_circuit)

    private:
        static opcode_dictionary _opcodes;
    };

};
