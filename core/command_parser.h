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

    typedef std::map<std::string, command_spec_t> command_table;

    class command_parser : public core::parser {
    public:
        static command_table& command_catalog();

        command_parser();

        ast_node_shared_ptr parse(const std::string& input) override;

    protected:
        ast_node_shared_ptr parse_command();

    private:
        static command_table _commands;
    };

};

