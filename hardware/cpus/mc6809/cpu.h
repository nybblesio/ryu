//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <rttr/type>
#include <hardware/integrated_circuit.h>
#include "opcode.h"
#include "assembly_language_parser.h"

namespace ryu::hardware::mc6809 {

    class cpu : public hardware::integrated_circuit {
    public:
        static const mnemonic_set& mnemonics();

        static const opcode_dictionary& opcodes();

        static void init();

        cpu();

        endianness::types endianess() const override;

        core::assembly_language_parser* assembler() override;

        RTTR_ENABLE(hardware::integrated_circuit)

    private:
        static mnemonic_set _mnemonics;
        static opcode_dictionary _opcodes;

        assembly_language_parser _parser {};
    };

};
