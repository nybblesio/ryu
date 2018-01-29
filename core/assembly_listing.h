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

    class assembly_listing {
    public:
        enum row_flags : uint8_t {
            none    = 0b00000000,
            include = 0b00000001,
            binary  = 0b00000010,
            macro   = 0b00000100
        };

        typedef uint8_t row_flags_t;

        assembly_listing();

        virtual ~assembly_listing() = default;

        core::data_table_t& table();

        void clear();

        void add_row(
                uint32_t line_number,
                const std::vector<uint32_t>& opcodes,
                row_flags_t flags,
                const std::string& source);

        void add_footer(
                uint32_t total_lines);

    private:
        core::data_table_t _table {};
    };

};

