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

#include <core/assembly_language_parser.h>

namespace ryu::hardware::mc6809 {

    class assembly_parser : public ryu::core::assembly_language_parser {
    public:
        assembly_parser() = default;

    protected:
        ryu::core::ast_node_shared_ptr on_parse_dialect() override;

    private:
    };

};

