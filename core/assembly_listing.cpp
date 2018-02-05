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
                25,
                25,
                alignment::horizontal::left,
                1,
                format_options::none
        });
        _table.headers.push_back({
                "Flags",
                5,
                5,
                alignment::horizontal::left,
                1,
                format_options::none
        });
        _table.headers.push_back({
                "Source",
                75,
                75,
                alignment::horizontal::left,
                1,
                format_options::style_codes
        });

        _table.footers.push_back({"Line Count", 15, 20});
    }

    void assembly_listing::reset() {
        while (!_scopes.empty())
            _scopes.pop();
        _table.rows.clear();
        _current_line = 1;
        _annotated_line_count = 0;
    }

    void assembly_listing::finalize() {
        _table.rows.push_back({
            {fmt::format("{} lines assembled", _annotated_line_count)}
        });
    }

    void assembly_listing::end_assembly_scope() {
        auto scope = current_scope();

        for (; scope->line_number < scope->input.source_lines.size(); scope->line_number++) {
            auto rows = format_rows(
                    scope->line_number,
                    0,
                    {},
                    assembly_listing::row_flags::none);
            for (const auto& row : rows)
                _table.rows.push_back(row);
        }

        if (!_scopes.empty())
            _scopes.pop();
    }

    void assembly_listing::annotate_line(
            uint32_t line_number,
            uint32_t address,
            const std::vector<uint8_t>& opcodes,
            assembly_listing::row_flags_t flags) {
        auto scope = current_scope();

        for (; scope->line_number < line_number; scope->line_number++) {
            auto rows = format_rows(
                    scope->line_number,
                    0,
                    {},
                    assembly_listing::row_flags::none);
            for (const auto& row : rows)
                _table.rows.push_back(row);
        }

        auto rows = format_rows(
                scope->line_number++,
                address,
                opcodes,
                flags);
        for (const auto& row : rows)
            _table.rows.push_back(row);
        _annotated_line_count += rows.size();
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
        row.columns.emplace_back(std::string(8, ' '));
        row.columns.emplace_back(std::string(25, ' '));

        std::string flag_chars(5, ' ');
        if (_scopes.size() > 1)
            flag_chars[1] = 'i';
        flag_chars[3] = 'e';
        row.columns.emplace_back(flag_chars);

        row.columns.emplace_back(fmt::format("<red>^ {}<>", error));
        if (line_number < _table.rows.size() - 1) {
            _table.rows.insert(
                    _table.rows.begin() + (line_number - 1),
                    row);
        } else {
            _table.rows.push_back(row);
        }
    }

    std::vector<data_table_row_t> assembly_listing::format_rows(
            uint32_t line_number,
            uint32_t address,
            const std::vector<uint8_t>& opcodes,
            assembly_listing::row_flags_t flags) {
        std::vector<data_table_row_t> rows {};

        core::data_table_row_t row {};
        row.columns.push_back(fmt::format("{:04d}", _current_line++));

        if ((flags & row_flags::address) == 0) {
            row.columns.emplace_back(8, ' ');
        } else {
            row.columns.push_back(fmt::format("{:08x}", address));
        }

        if (opcodes.empty()) {
            row.columns.emplace_back(25, ' ');
        }
        else {
            std::stringstream stream;
            auto data_count = std::min<size_t>(opcodes.size(), 8);
            for (auto i = 0; i < data_count; i++) {
                stream << fmt::format("{:02x} ", opcodes[i]);
            }
            row.columns.push_back(stream.str());
        }

        std::string flag_chars(5, ' ');
        if ((flags & row_flags::binary) != 0)
            flag_chars[0] = 'b';
        if ((flags & row_flags::include) != 0 || _scopes.size() > 1)
            flag_chars[1] = 'i';
        if ((flags & row_flags::macro) != 0)
            flag_chars[2] = 'm';
        if ((flags & row_flags::error) != 0)
            flag_chars[3] = 'e';
        row.columns.push_back(flag_chars);

        auto scope = current_scope();
        if (line_number < scope->input.source_lines.size())
            row.columns.push_back(scope->input.source_lines[line_number - 1]);
        else
            row.columns.emplace_back(75, ' ');

        rows.push_back(row);

        auto create_overflow_row = [&](auto addr, auto data) -> data_table_row_t {
            data_table_row_t overflow_row {};
            overflow_row.columns.emplace_back(4, ' ');
            overflow_row.columns.push_back(fmt::format("{:08x}", addr));
            overflow_row.columns.emplace_back(data);

            std::string overflow_flags(5, ' ');
            if ((flags & row_flags::binary) != 0)
                overflow_flags[0] = 'b';
            if (_scopes.size() > 1)
                overflow_flags[1] = 'i';
            overflow_flags[4] = 'c';
            overflow_row.columns.emplace_back(overflow_flags);

            overflow_row.columns.emplace_back(75, ' ');
            return overflow_row;
        };

        auto index = 8;
        std::stringstream stream;
        address += 8;
        while (index < opcodes.size()) {
            stream << fmt::format("{:02x} ", opcodes[index]);
            ++index;
            if (index % 8 == 0) {
                rows.push_back(create_overflow_row(address, stream.str()));
                stream.str(std::string());
                address += 8;
            }
        }

        auto stream_str = stream.str();
        if (!stream_str.empty()) {
            rows.push_back(create_overflow_row(address, stream.str()));
        }

        return rows;
    }

    core::data_table_t& assembly_listing::table() {
        return _table;
    }

    assembly_listing_scope_t* assembly_listing::current_scope() {
        if (_scopes.empty())
            return nullptr;
        return &_scopes.top();
    }

    void assembly_listing::begin_assembly_scope(const parser_input_t& source) {
        assembly_listing_scope_t scope {1, source};
        _scopes.push(scope);
    }

}