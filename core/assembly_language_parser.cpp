//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <regex>
#include <boost/algorithm/string.hpp>
#include "assembly_language_parser.h"

namespace ryu::core {

    ast_node_shared_ptr assembly_language_parser::parse_dialect() {
        return on_parse_dialect();
    }

    ast_node_shared_ptr assembly_language_parser::parse(const parser_input_t& input) {
        reset(input);
        return parse_dialect();
    }

}