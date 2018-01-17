//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include <vector>
#include <sstream>
#include <fmt/format.h>
#include "string_support.h"

namespace ryu {

    std::string word_wrap(
            std::string text,
            size_t width,
            size_t right_pad,
            const char& fill) {
        size_t line_begin = 0;

        while (line_begin < text.size()) {
            const auto ideal_end = line_begin + width;
            size_t line_end = ideal_end <= text.size() ?
                              ideal_end :
                              text.size() - 1;

            if (line_end == text.size() - 1)
                ++line_end;
            else if (std::isspace(text[line_end])) {
                text[line_end++] = '\n';
                for (size_t i = 0; i < right_pad; i++)
                    text.insert(line_end++, 1, fill);
            } else {
                auto end = line_end;
                while (end > line_begin && !std::isspace(text[end]))
                    --end;

                if (end != line_begin) {
                    line_end = end;
                    text[line_end++] = '\n';
                    for (size_t i = 0; i < right_pad; i++)
                        text.insert(line_end++, 1, fill);
                } else {
                    text.insert(line_end++, 1, '\n');
                    for (size_t i = 0; i < right_pad; i++)
                        text.insert(line_end++, 1, fill);
                }
            }

            line_begin = line_end;
        }

        return text;
    }

    void hex_dump(
            const void* data,
            size_t size,
            uint8_t bytes,
            std::vector<std::vector<std::string>>& results) {
        auto* buf = (unsigned char*)data;
        int i, j;
        for (i = 0; i < size; i += bytes) {
            std::vector<std::string> line{};
            line.push_back(fmt::format("{:06x}", i));
            std::stringstream byte_stream;
            std::stringstream ascii_stream;
            for (j = 0; j < bytes; j++) {
                byte_stream << fmt::format("{:02x} ", buf[i + j]);
                ascii_stream << (char) (isprint(buf[i + j]) ? buf[i + j] : '.');
            }
            line.push_back(byte_stream.str());
            line.push_back(ascii_stream.str());
            results.push_back(line);
        }
    }

    std::pair<std::string, std::string> size_to_units(size_t size) {
        auto i = 0;
        const char* units[] = {"bytes", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
        while (size > 1024) {
            size /= 1024;
            i++;
        }
        return std::make_pair(i > 0 ? fmt::format("{}.{}", i, size) : fmt::format("{}", size), units[i]);
    }

    std::string list_to_string(const std::vector<std::string>& list, const char& sep) {
        std::stringstream stream;

        for (auto i = 0; i < list.size(); i++) {
            if (i > 0)
                stream << sep;
            stream << list[i];
        }

        return stream.str();
    }

    std::vector<std::string> string_to_list(const std::string& value, const char& sep) {
        std::vector<std::string> list;

        std::istringstream f(value);
        std::string s;
        while (std::getline(f, s, sep)) {
            list.push_back(s);
        }

        return list;
    }

}