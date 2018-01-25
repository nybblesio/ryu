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

#include <string>
#include "parser.h"

namespace ryu::core {

    class assembly_language_parser : public core::parser {
    public:
        assembly_language_parser() = default;

        virtual ~assembly_language_parser() = default;

        ast_node_shared_ptr parse(const std::string& input) override;

    protected:
        ast_node_shared_ptr parse_dialect();

        virtual ast_node_shared_ptr on_parse_dialect() = 0;

    private:
    };

};

