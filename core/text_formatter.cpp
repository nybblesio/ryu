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

namespace ryu::core {

    formatted_text_t text_formatter::format_text_left_padded(
            const std::string& text,
            uint32_t width,
            const char& fill) {
        auto formatted_text = format_text(text);
        if (width < formatted_text.actual_length)
            return formatted_text;
        formatted_text.spans.push_back(
            {
               "",
               std::string(width - formatted_text.actual_length, fill)
            });
        return formatted_text;
    }

    formatted_text_t text_formatter::format_text_right_padded(
            const std::string& text,
            uint32_t width,
            const char& fill) {
        auto formatted_text = format_text(text);
        if (width < formatted_text.actual_length)
            return formatted_text;
        formatted_text.spans.insert(
            formatted_text.spans.begin(),
            {
                "",
                std::string(width - formatted_text.actual_length, fill)
            });
        return formatted_text;
    }

    formatted_text_t text_formatter::format_text(const std::string& text) {
        formatted_text_t formatted_text {};
        formatted_text_span_t current_span {};

        auto token = text.cbegin();
        while (token != text.cend()) {
            if (*token == '\n') {
                if (!current_span.attr_code.empty() || !current_span.text.empty()) {
                    formatted_text.spans.push_back(current_span);
                    current_span.text = "";
                }

                formatted_text.spans.push_back({"newline", ""});
                ++token;

                continue;
            }

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

                if (!current_span.attr_code.empty() || !current_span.text.empty())
                    formatted_text.spans.push_back(current_span);

                current_span.attr_code = code.empty() ? "reset" : code;
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

        if (!current_span.attr_code.empty() || !current_span.text.empty())
            formatted_text.spans.push_back(current_span);

        return formatted_text;
    }

    std::string text_formatter::formatted_text_to_string(const formatted_text_t& formatted_text) {
        std::stringstream stream;
        for (const auto& span : formatted_text.spans) {
            if (!span.attr_code.empty())
                stream << "<" << span.attr_code << ">";
            stream << span.text;
        }
        return stream.str();
    }

}