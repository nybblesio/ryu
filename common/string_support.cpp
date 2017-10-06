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

    std::string hex_dump(const void* data, size_t size) {
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