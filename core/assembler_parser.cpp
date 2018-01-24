//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "assembler_parser.h"

namespace ryu::core {

    assembler_parser::assembler_parser() {
    }

    ast_node_shared_ptr assembler_parser::parse(const std::string& input) {
        reset(input);
        return parse_assembly();
    }

    ast_node_shared_ptr assembler_parser::parse_assembly() {
        return nullptr;
    }

}