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

    class evaluator {
    public:
        evaluator() = default;

        core::symbol_table* symbol_table();

        void symbol_table(core::symbol_table* value);

        variant_t evaluate(core::result& result, const ast_node_t* node);

    protected:
        void error(core::result& result, const std::string& code, const std::string& message);

    private:
        core::symbol_table* _symbol_table = nullptr;
    };

};

