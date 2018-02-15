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
#include "core_types.h"

namespace ryu::core {

    struct formatted_text_t {
        uint32_t actual_length {};
        std::string stripped_text {};
        attr_span_list_t spans {};

        void add_empty_span(const core::code_to_attr_dict& code_mapper);
    };

    typedef std::vector<formatted_text_t> formatted_text_list;

    class text_formatter {
    public:
        static attr_span_t span_for_code(
                const code_to_attr_dict& mapper,
                const std::string& code,
                const std::string& text);

        static formatted_text_t format_text(
                const code_to_attr_dict& mapper,
                attr_t& attr,
                const std::string& text);

        static formatted_text_t format_text_left_padded(
                const code_to_attr_dict& mapper,
                attr_t& attr,
                const std::string& text,
                uint32_t width,
                const char& fill = ' ');

        static formatted_text_t format_text_right_padded(
                const code_to_attr_dict& mapper,
                attr_t& attr,
                const std::string& text,
                uint32_t width,
                const char& fill = ' ');
    };

};

