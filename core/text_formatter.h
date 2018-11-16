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

#include <cstdint>
#include <string>
#include <vector>

namespace ryu::core {

    struct formatted_text_span_t {
        std::string attr_code {};
        std::string text {};
    };

    using formatted_text_span_list = std::vector<formatted_text_span_t>;

    struct formatted_text_t {
        uint32_t actual_length {};
        std::string stripped_text {};
        formatted_text_span_list spans {};
    };

    using formatted_text_list = std::vector<formatted_text_t>;

    class text_formatter {
    public:
        static formatted_text_t format_text_left_padded(
                const std::string& text,
                uint32_t width,
                const char& fill = ' ');

        static formatted_text_t format_text_right_padded(
                const std::string& text,
                uint32_t width,
                const char& fill = ' ');

        static formatted_text_t format_text(const std::string& text);

        static std::string formatted_text_to_string(const formatted_text_t& formatted_text);
    };

};

