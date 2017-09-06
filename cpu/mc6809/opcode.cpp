//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include "opcode.h"

namespace ryu::cpu::mc6809 {

    opcode::opcode(
            uint16_t op,
            uint8_t cycles,
            uint8_t mode,
            const std::set<std::string>& mnemonics) : _op(op),
                                                      _cycles(cycles),
                                                      _mode(mode),
                                                      _mnemonics(mnemonics) {
    }

}