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

#include "parser.h"

namespace ryu::core {

    class assembler_parser  : public core::parser {
    public:
        assembler_parser();

        ast_node_shared_ptr parse(const std::string& input) override;

    protected:
        ast_node_shared_ptr parse_assembly();

    private:
    };

};

