//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "assembler.h"

namespace ryu::core {

    bool assembler::assemble(
            core::result& result,
            std::iostream& stream) {
        return false;
    }

    core::symbol_table* assembler::symbol_table() {
        return _symbol_table;
    }

    void assembler::symbol_table(core::symbol_table* value) {
        _symbol_table = value;
        _parser.symbol_table(_symbol_table);
    }

}