//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <hardware/cpu.h>
#include "opcode.h"

namespace ryu::cpu::mc6809 {

    class cpu : public hardware::cpu {
    public:
        static void init();

        cpu(int id, const std::string& name);

    private:
        static opcode_dictionary _opcodes;
    };

};
