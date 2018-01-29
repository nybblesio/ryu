//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <iostream>
#include "assembler.h"

namespace ryu::core {

    bool assembler::assemble(
            core::result& result,
            std::string& input) {
        auto program_node = _parser.parse(input);
        auto parser_result = _parser.result();
        if (program_node == nullptr && parser_result.is_failed()) {
            for (auto& msg : parser_result.messages())
                result.add_message(
                        msg.code(),
                        msg.message(),
                        msg.details(),
                        msg.is_error());
            return false;
        }

        if (program_node != nullptr)
            program_node->serialize(std::cout);

        return !result.is_failed();
    }

    core::symbol_table* assembler::symbol_table() {
        return _symbol_table;
    }

    void assembler::symbol_table(core::symbol_table* value) {
        _symbol_table = value;
        _parser.symbol_table(_symbol_table);
    }

}