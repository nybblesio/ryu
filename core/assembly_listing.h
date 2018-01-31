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
            macro   = 0b00000100,
            error   = 0b00001000
        };

        typedef uint8_t row_flags_t;

        assembly_listing();

        virtual ~assembly_listing() = default;

        core::data_table_t& table();

        void end_assembly();

        void annotate_line(
                uint32_t line_number,
                uint32_t address,
                const std::vector<uint8_t>& opcodes,
                row_flags_t flags);

        void annotate_line_error(
                uint32_t line_number,
                const std::string& error);

        void begin_assembly(const std::string& source);

    private:
        data_table_row_t format_line(
                uint32_t line_number,
                uint32_t address,
                const std::vector<uint8_t>& opcodes,
                row_flags_t flags);

    private:
        uint32_t _current_line = 1;
        core::data_table_t _table {};
        uint32_t _annotated_line_count = 0;
        std::vector<std::string> _lines {};
    };

};

