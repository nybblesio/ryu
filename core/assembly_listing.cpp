//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "assembly_listing.h"

namespace ryu::core {

    assembly_listing::assembly_listing() {
        using format_options = core::data_table_column_t::format_options;

        _table.line_spacing = 0;
        _table.headers.push_back({
                "Line",
                4,
                4,
                alignment::horizontal::left,
                1,
                format_options::none
        });
        _table.headers.push_back({
                "Addr",
                8,
                8,
                alignment::horizontal::left,
                1,
                format_options::none
        });
        _table.headers.push_back({
                "Opcodes/Data",
                20,
                20,
                alignment::horizontal::left,
                1,
                format_options::word_wrap
        });
        _table.headers.push_back({
                "Flags",
                5,
                5,
                alignment::horizontal::left,
                1,
                format_options::style_codes
        });
        _table.headers.push_back({
                "Source",
                35,
                35,
                alignment::horizontal::left,
                1,
                format_options::style_codes
        });

        _table.footers.push_back({"Line Count", 15, 20});
    }

    void assembly_listing::end_assembly() {
        for (; _current_line < _lines.size(); _current_line++) {
            auto line = format_line(
                    _current_line,
                    0,
                    {},
                    assembly_listing::row_flags::none);
            _table.rows.push_back(line);
        }
        _table.rows.push_back({
              {fmt::format("{} lines assembled", _annotated_line_count)}
        });
    }

    void assembly_listing::annotate_line(
            uint32_t line_number,
            uint32_t address,
            const std::vector<uint8_t>& opcodes,
            assembly_listing::row_flags_t flags) {
        for (; _current_line < line_number; _current_line++) {
            auto line = format_line(
                    _current_line,
                    0,
                    {},
                    assembly_listing::row_flags::none);
            _table.rows.push_back(line);
        }

        auto line = format_line(
                _current_line++,
                address,
                opcodes,
                flags);
        _table.rows.push_back(line);
        _annotated_line_count++;
    }

    void assembly_listing::annotate_line_error(
            uint32_t line_number,
            const std::string& error) {
        annotate_line(
                line_number,
                0,
                {},
                assembly_listing::row_flags::error);

        core::data_table_row_t row {};
        row.columns.push_back(fmt::format("{:04d}", line_number));
        row.columns.emplace_back("");
        row.columns.emplace_back("");
        row.columns.emplace_back("<red>e<>");
        row.columns.emplace_back(fmt::format("<red>^ {}<>", error));
        if (line_number < _table.rows.size()) {
            _table.rows.insert(
                    _table.rows.begin() + (line_number - 1),
                    row);
        } else {
            _table.rows.push_back(row);
        }
    }

    data_table_row_t assembly_listing::format_line(
            uint32_t line_number,
            uint32_t address,
            const std::vector<uint8_t>& opcodes,
            assembly_listing::row_flags_t flags) {
        core::data_table_row_t row {};
        row.columns.push_back(fmt::format("{:04d}", line_number));

        if ((flags & row_flags::none) != 0
        ||  (flags & row_flags::error) != 0
        ||  address == 0) {
            row.columns.emplace_back("");
        } else {
            row.columns.push_back(fmt::format("{:08x}", address));
        }

        std::string opcode_hex;
        for (const auto& opcode : opcodes)
            opcode_hex += fmt::format("{:02x} ", opcode);
        row.columns.push_back(opcode_hex);

        std::string flag_chars;
        if ((flags & row_flags::binary) != 0)
            flag_chars += 'b';
        if ((flags & row_flags::include) != 0)
            flag_chars += 'i';
        if ((flags & row_flags::macro) != 0)
            flag_chars += 'm';
        if ((flags & row_flags::error) != 0)
            flag_chars += "<red>e<>";
        row.columns.push_back(flag_chars);

        if (line_number < _lines.size())
            row.columns.push_back(_lines[line_number - 1]);
        else
            row.columns.emplace_back("");


        return row;
    }

    core::data_table_t& assembly_listing::table() {
        return _table;
    }

    void assembly_listing::begin_assembly(const std::string& source) {
        _table.rows.clear();
        _lines.clear();

        _current_line = 1;
        _annotated_line_count = 0;

        std::stringstream stream;
        stream << source;
        std::string line;
        while (std::getline(stream, line)) {
            _lines.push_back(line);
        }
    }
}