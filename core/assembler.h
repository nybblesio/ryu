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

#include <hardware/component.h>
#include "assembler_parser.h"
#include "evaluator.h"
#include "assembly_listing.h"

namespace ryu::core {

    class assembler {
    public:
        assembler() = default;

        virtual ~assembler() = default;

        bool assemble(
                core::result& result,
                std::string& input);

        bool load_target(
                core::result& result,
                const std::string& name);

        hardware::component* target();

        core::assembly_listing& listing();

        uint32_t location_counter() const;

        core::symbol_table* symbol_table();

        void location_counter(uint32_t value);

        void write_data(const std::string& value);

        void symbol_table(core::symbol_table* value);

        void write_data(directive_t::data_sizes size, uint32_t value);

    private:
        core::evaluator _evaluator {};
        uint32_t _location_counter = 0;
        core::assembler_parser _parser {};
        core::assembly_listing _listing {};
        hardware::component* _target = nullptr;
        core::symbol_table* _symbol_table = nullptr;
    };

};

