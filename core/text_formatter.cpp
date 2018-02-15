//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <sstream>
#include "text_formatter.h"
#include "font_family.h"

namespace ryu::core {

    ///////////////////////////////////////////////////////////////////////////

    void formatted_text_t::add_empty_span(const code_to_attr_dict& code_mapper) {
        spans.push_back(text_formatter::span_for_code(code_mapper, "", ""));
    }

    ///////////////////////////////////////////////////////////////////////////

    attr_span_t text_formatter::span_for_code(
            const code_to_attr_dict& mapper,
            const std::string& code,
            const std::string& text) {
        attr_t attr {};
        attr_span_t span {attr, text};

        if (!code.empty()) {
            auto it = mapper.find(code);
            if (it != mapper.end()) {
                it->second(attr);
            }
        }

        return span;
    }

    formatted_text_t text_formatter::format_text(
            const code_to_attr_dict& mapper,
            attr_t& attr,
            const std::string& text) {
        formatted_text_t formatted_text {};
        attr_t default_attr = attr;
        attr_span_t current_span {};

        auto token = text.cbegin();
        while (token != text.cend()) {
            if (*token == '\\') {
                if (token + 1 != text.cend()) {
                    if (*(token + 1) == '<' || *(token + 1) == '>') {
                        ++token;
                        goto _escaped;
                    }
                }
            }

            if (*token == '<') {
                auto mark = token;
                std::string code;
                while (*(++token) != '>') {
                    if (token == text.cend()) {
                        token = mark;
                        goto _escaped;
                    }
                    code += *token;
                }

                if (!current_span.text.empty()) {
                    current_span.attr = attr;
                    formatted_text.spans.push_back(current_span);
                }

                auto reset = false;
                if (!code.empty()) {
                    auto it = mapper.find(code);
                    if (it != mapper.end()) {
                        it->second(attr);
                    } else {
                        reset = true;
                    }
                } else {
                    reset = true;
                }

                if (reset) {
                    attr.color = default_attr.color;
                    attr.flags = core::font::flags::none;
                    attr.style = core::font::styles::normal;
                }

                current_span.attr = {};
                current_span.text = "";
                ++token;

                continue;
            }

            _escaped:
            current_span.text += *token;
            formatted_text.stripped_text += *token;

            ++token;
            ++formatted_text.actual_length;
        }

        if (!current_span.text.empty()) {
            current_span.attr = attr;
            formatted_text.spans.push_back(current_span);
        }

        return formatted_text;
    }

    formatted_text_t text_formatter::format_text_left_padded(
            const code_to_attr_dict& mapper,
            attr_t& attr,
            const std::string& text,
            uint32_t width,
            const char& fill) {
        auto formatted_text = format_text(mapper, attr, text);
        if (width < formatted_text.actual_length)
            return formatted_text;
        formatted_text.spans.push_back({attr, std::string(width - formatted_text.actual_length, fill)});
        return formatted_text;
    }

    formatted_text_t text_formatter::format_text_right_padded(
            const code_to_attr_dict& mapper,
            attr_t& attr,
            const std::string& text,
            uint32_t width,
            const char& fill) {
        auto formatted_text = format_text(mapper, attr, text);
        if (width < formatted_text.actual_length)
            return formatted_text;
        formatted_text.spans.insert(
            formatted_text.spans.begin(),
            {
                {},
                std::string(width - formatted_text.actual_length, fill)
            });
        return formatted_text;
    }

}