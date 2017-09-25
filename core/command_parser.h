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

#include <stack>
#include <vector>
#include <string>
#include <core/result.h>
#include "parser.h"

namespace ryu::core {

    class command_parser : public core::parser {
    public:
        command_parser();

        ast_node_t* parse(const std::string& input) override;

    protected:
        ast_node_t* parse_command();

    private:
        static command_table _commands;
    };

};

