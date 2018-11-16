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

#include <queue>
#include "core_types.h"
#include "parser_types.h"

namespace ryu::core {

    struct assembly_listing_scope_t {
        uint32_t line_number = 1;
        parser_input_t input;
    };

    class assembly_listing {
    public:
        enum row_flags : uint8_t {
            none    = 0b00000000,
            include = 0b00000001,
            binary  = 0b00000010,
            macro   = 0b00000100,
            error   = 0b00001000,
            address = 0b00010000
        };

        using row_flags_t = uint8_t;

        assembly_listing();

        virtual ~assembly_listing() = default;

        void reset();

        void finalize();

        void annotate_line(
                uint32_t line_number,
                uint32_t address,
                const byte_list& opcodes,
                row_flags_t flags);

        void annotate_line_error(
                uint32_t line_number,
                const std::string& error);

        void end_assembly_scope();

        core::data_table_t& table();

        void begin_assembly_scope(const parser_input_t& source);

    private:
        assembly_listing_scope_t* current_scope();

        std::vector<data_table_row_t> format_rows(
                uint32_t line_number,
                uint32_t address,
                const byte_list& opcodes,
                row_flags_t flags);

    private:
        uint32_t _current_line = 1;
        core::data_table_t _table {};
        uint32_t _annotated_line_count = 0;
        std::stack<assembly_listing_scope_t> _scopes {};
    };

};

