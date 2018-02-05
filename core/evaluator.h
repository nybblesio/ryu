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

    class assembler;

    class evaluator {
    public:
        explicit evaluator(core::assembler* assembler);

        bool pass1_transform(
                core::result& result,
                const core::ast_node_shared_ptr& program_node);

        variant_t evaluate(
                core::result& result,
                const ast_node_shared_ptr& node);

        core::symbol_table* symbol_table();

        void symbol_table(core::symbol_table* value);

    protected:
        void error(
                core::result& result,
                const std::string& code,
                const std::string& message);

    private:
        bool pass1_transform_node(
                core::result& result,
                const core::ast_node_shared_ptr& node);

        bool transform_identifier_node(
                core::result& result,
                const core::ast_node_shared_ptr& node);

        bool transform_directive_node(
                core::result& result,
                const core::ast_node_shared_ptr& node);

    private:
        core::assembler* _assembler = nullptr;
        core::symbol_table* _symbol_table = nullptr;
    };

};

