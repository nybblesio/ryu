//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "hex_formatter.h"

namespace ryu::core {

    std::string hex_formatter::dump_to_string(
            const void* data,
            size_t size) {
        auto* buf = (unsigned char*)data;
        int i, j;
        std::stringstream stream;
        for (i=0; i<size; i+=16) {
            stream << fmt::format("{:06x}: ", i);
            for (j=0; j<16; j++)
                if (i+j < size)
                    stream << fmt::format("{:02x} ", buf[i+j]);
                else
                    stream << "   ";
            stream << " ";
            for (j=0; j<16; j++)
                if (i+j < size)
                    stream << (char)(isprint(buf[i+j]) ? buf[i+j] : '.');
            stream << "\n";
        }
        return stream.str();
    }

    core::data_table_t hex_formatter::dump_to_table(
            const void* data,
            size_t size,
            uint8_t bytes) {
        data_table_t table {};
        table.headers.push_back({"Addr", 4, 4});

        for (auto i = 0; i < bytes; i++)
            table.headers.push_back({fmt::format("{:02}", i), 2, 2});

        for (auto i = 0; i < bytes; i++) {
            table.headers.push_back({
                "",
                1,
                1,
                core::alignment::horizontal::types::left,
                0
            });
        }

        table.footers.push_back({"Bytes", 20, 20});

        auto* buf = (unsigned char*)data;
        int i, j;
        for (i = 0; i < size; i += bytes) {
            data_table_row_t row {};
            row.columns.push_back(fmt::format("{:04x}", i));
            for (j = 0; j < bytes; j++) {
                row.columns.push_back(fmt::format("{:02x} ", buf[i + j]));
            }
            for (j = 0; j < bytes; j++) {
                row.columns.push_back(fmt::format(
                    "{}",
                    (char) (isprint(buf[i + j]) ? buf[i + j] : '.')));
            }
            table.rows.push_back(row);
        }

        table.rows.push_back({{fmt::format("{} bytes", size)}});

        return table;
    }

}