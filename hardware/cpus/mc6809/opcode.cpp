//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include "opcode.h"
#include <utility>

namespace ryu::hardware::mc6809 {

    opcode::opcode(
            uint16_t op,
            uint8_t cycles,
            uint8_t mode,
            std::set<std::string> mnemonics) : _op(op),
                                               _cycles(cycles),
                                               _mnemonics(std::move(mnemonics)),
                                               _mode(mode) {
    }

}