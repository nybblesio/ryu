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

#include "core_types.h"

namespace ryu::core {

    class symbol_table {
    public:
        symbol_table() = default;

        ast_node_t* get(const std::string& name) const;

        void put(const std::string& name, ast_node_t* value);

    private:
        symbol_dict _symbols;
    };

};

