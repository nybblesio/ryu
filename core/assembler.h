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

#include "assembler_parser.h"

namespace ryu::core {

    class assembler {
    public:
        assembler() = default;

        virtual ~assembler() = default;

        bool assemble(
                core::result& result,
                std::string& input);

        core::symbol_table* symbol_table();

        void symbol_table(core::symbol_table* value);

    private:
        uint32_t _location_counter = 0;
        core::assembler_parser _parser {};
        core::symbol_table* _symbol_table = nullptr;
    };

};

