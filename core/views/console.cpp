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
#include <fmt/format.h>
#include "console.h"

// TODO
//
// - support select, cut, copy, paste (use text_editor implementation for start)
// - bug fixes

namespace ryu::core {

    console::command_action_dict console::_handlers = {
        {
            "clear",
            [](core::console& console, auto params) {
                console._document.clear();
                console.caret_home();
                console._caret.row(0);
                return false;
            }
        },
        {
            "edit_machine",
            [](core::console& console, auto params) {
                console.transition_to("edit_machine", params);
                return true;
            }
        },
        {
            "edit_source",
            [](core::console& console, auto params) {
                console.transition_to("edit_source", params);
                return true;
            }
        },
        {
            "edit_memory",
            [](core::console& console, auto params) {
                console.transition_to("edit_memory", params);
                return true;
            }
        },
    };

    console::console(const std::string& name) : core::view(core::view::types::container, name),
                                                          _caret("console-caret") {
    }

    void console::caret_end() {
        _caret.column(_metrics.page_width);
    }

    void console::caret_home() {
        _caret.column(0);
    }

    void console::initialize() {
        _color = fg_color();

        _document.default_attr(core::attr_t {_color, core::font::styles::normal, core::font::flags::none});
        _document.document_size(128, 128);
        _document.clear();

        _caret.overwrite();
        _caret.initialize(0, 0);
        _caret.palette(palette());
        _caret.font_family(font_family());
        _caret.on_caret_changed([&]() {
            raise_caret_changed();
        });

        add_child(&_caret);
        margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
    }

    void console::update(uint32_t) {
        switch (_state) {
            case input: {
                break;
            }
            case pre_processing: {
                on_pre_process_command();
                break;
            }
            case processing: {
                on_process_command();
                break;
            }
            case post_processing: {
                on_post_process_command();
                break;
            }
            case resume_processing: {
                on_resume_process_command();
                break;
            }
            case wait: {
                break;
            }
            default:
                break;
        }
    }

    void console::raise_caret_changed() {
        if (_caret_changed_callback != nullptr)
            _caret_changed_callback(_caret);
    }

    void console::caret_up(uint8_t rows) {
        _caret.up(rows);
    }

    void console::caret_down(uint8_t rows) {
        if (_caret.down(rows))
            _document.shift_up();
    }

    void console::calculate_page_metrics() {
        auto rect = bounds();
        if (rect.empty())
            return;
        _metrics.page_width = static_cast<uint8_t>(rect.width() / font_face()->width);
        _metrics.page_height = static_cast<uint8_t>(rect.height() / font_face()->line_height);
    }

    void console::caret_color(uint8_t color) {
        _caret.fg_color(color);
    }

    bool console::caret_left(uint8_t columns) {
        if (_caret.left(columns)) {
            caret_up();
            caret_end();
            return true;
        }
        return false;
    }

    bool console::caret_right(uint8_t columns) {
        if (_caret.right(columns)) {
            caret_down();
            caret_home();
            return true;
        }
        return false;
    }

    void console::code_mapper(const code_to_attr_dict& value) {
        _code_mapper = value;
    }

    void console::on_resize(const core::rect& context_bounds) {
        core::view::on_resize(context_bounds);
        calculate_page_metrics();
        _caret.page_size(_metrics.page_height, _metrics.page_width);
        _document.page_size(_metrics.page_height, _metrics.page_width);
    }

    void console::on_draw(core::renderer& surface) {
        auto bounds = client_bounds();
        auto pal = *palette();

        // XXX: this may not be correct because font metrics may change
        auto face = font_face();
        auto y = bounds.top();
        for (uint32_t row = 0; row < _metrics.page_height; row++) {
            auto x = bounds.left();
            auto chunks = _document.get_line_chunks(row, 0, _metrics.page_width);
            for (const auto& chunk : chunks) {
                auto width = static_cast<int32_t>(face->width * chunk.text.length());
                auto color = pal[chunk.attr.color];

                if ((chunk.attr.flags & core::font::flags::reverse) != 0) {
                    surface.set_color(color);
                    color = pal[bg_color()];
                    surface.fill_rect(core::rect{x, y, width, face->line_height});
                }

                font_style(chunk.attr.style);
                surface.draw_text(font_face(), x, y, chunk.text, color);
                x += width;
            }
            y += face->line_height;
        }
    }

    std::string console::find_command_string() {
        std::stringstream cmd;

        // first, seek backwards to a null value
        while (true) {
            caret_left();
            if (_caret.row() == 0 && _caret.column() == 0)
                break;
            auto element = _document.get(_caret.row(), _caret.column());
            if (element == nullptr || element->value == 0)
                break;
        }

        // second, scan forward to the next null value to build the command
        //caret_right();
        while (true) {
            caret_right();
            auto element = _document.get(_caret.row(), _caret.column());
            if (element == nullptr || element->value == 0)
                break;
            cmd << element->value;
        }

        // finally, if we got something, try to execute it
        return cmd.str();
    }

    void console::on_process_command() {
        const auto& result = _command_result_queue.front();

        caret_down();
        caret_home();

        const auto& list = result.messages();
        while (_remaining_lines > 0
            && _current_result_message_index < list.size()) {

            const auto& msg = list[_current_result_message_index++];
            if (msg.type() == core::result_message::types::data)
                continue;

            auto error_part = msg.is_error() ?
                "<bold><red>ERROR:<> " :
                "";
            auto line_count = write_message(fmt::format("{0}{1}",
                error_part,
                msg.message()));

            _remaining_lines -= line_count;
        }

        if (_current_result_message_index < list.size()) {
            write_message("<rev><bold> MORE (SPACE to continue) <>", false);
            _state = states::wait;
        } else {
            _state = states::post_processing;
        }
    }

    std::string console::get_alignment_format(
            core::alignment::horizontal::types value) {
        std::string format;
        switch (value) {
            case alignment::horizontal::right:
                format = "{:>{}}";
                break;
            case alignment::horizontal::center:
                format = "{:^{}}";
                break;
            default:
                format = "{:<{}}";
                break;
        }
        return format;
    }

    void console::scale_columns(std::vector<data_table_column_t>& columns) {
        auto target_width = _metrics.page_width - 4;

        auto get_working_size = [&]() {
            auto working_size = 0;
            auto clamped_count = 0;
            for (auto& col : columns) {
                if (col.width == 0)
                    col.width = col.max_width;
                else if (col.width == col.min_width)
                    clamped_count++;
                working_size += col.width + 1;
            }
            return clamped_count == columns.size() ?
                   target_width :
                   working_size;
        };

        while (true) {
            auto working_size = get_working_size();
            if (working_size <= target_width)
                break;
            for (auto& col : columns) {
                col.width--;
                if (col.width < col.min_width)
                    col.width = col.min_width;
            }
        }
    }

    void console::format_command_result(const parameter_variant_t& param) {
        std::vector<core::formatted_text_t> lines {};

        switch (param.which()) {
            case parameter_dict_types::table: {
                auto table = boost::get<core::data_table_t>(param);
                scale_columns(table.headers);
                scale_columns(table.footers);

                core::formatted_text_t header_line {};
                header_line.spans.push_back({"rev", " "});
                for (size_t i = 0; i < table.headers.size(); i++) {
                    auto column_pad = i < table.headers.size() - 1 ? 1 : 0;
                    auto& col = table.headers[i];
                    header_line.spans.push_back({
                        "",
                        fmt::format(
                                get_alignment_format(col.alignment),
                                col.text.substr(0, col.width),
                                col.width + column_pad)
                    });
                }
                header_line.spans.push_back({"", " "});
                if (table.rows.size() - 1 == 0) {
                    header_line.spans.push_back({"reset"});
                    header_line.spans.push_back({"newline"});
                    header_line.spans.push_back({"italic", " No results."});
                }
                lines.push_back(header_line);

                for (size_t i = 0; i < table.rows.size() - 1; i++) {
                    const auto& row = table.rows[i];
                    core::formatted_text_t row_line{};
                    row_line.spans.push_back({"", " "});

                    for (size_t j = 0; j < row.columns.size(); j++) {
                        auto column_pad = j < row.columns.size() - 1 ? 1 : 0;
                        const auto& header = table.headers[j];
                        const auto& col = row.columns[j];
                        row_line.spans.push_back({
                             "",
                             fmt::format(
                                     get_alignment_format(header.alignment),
                                     col.substr(0, header.width),
                                     header.width + column_pad)
                        });
                    }

                    lines.push_back(row_line);
                }

                core::formatted_text_t footer_line {};
                footer_line.spans.push_back({"rev", " "});
                const auto& footer_data_row = table.rows[table.rows.size() - 1];
                for (size_t i = 0; i < footer_data_row.columns.size(); i++) {
                    auto& footer = table.footers[i];
                    const auto& col = footer_data_row.columns[i];
                    auto column_pad = i < footer_data_row.columns.size() - 1 ? 1 : 0;

                    footer_line.spans.push_back({
                        "",
                        fmt::format(
                                get_alignment_format(footer.alignment),
                                col.substr(0, footer.width),
                                footer.width + column_pad)
                    });
                }
                lines.push_back(footer_line);
                break;
            }
            case parameter_dict_types::string: {
                auto value = boost::get<std::string>(param);
                core::formatted_text_t line {};
                line.spans.push_back({"", value});
                lines.push_back(line);
                break;
            }
            case parameter_dict_types::integer32: {
                auto value = boost::get<uint32_t>(param);
                core::formatted_text_t line {};
                line.spans.push_back({"", std::to_string(value)});
                lines.push_back(line);
                break;
            }
            case parameter_dict_types::boolean: {
                auto value = boost::get<bool>(param);
                core::formatted_text_t line {};
                line.spans.push_back({"", std::to_string(value)});
                lines.push_back(line);
                break;
            }
            default: {
                break;
            }
        }

        caret_down();
        caret_home();

        for (size_t i = 0; i < lines.size(); i++) {
            write_message(lines[i], i < lines.size() - 1);
        }
    }

    void console::on_pre_process_command() {
        auto& result = _command_result_queue.front();

        auto command_result_msg = result.find_code("command_result");
        if (command_result_msg != nullptr) {
            auto params = command_result_msg->params();
            for (auto& param : params) {
                caret_down();
                caret_home();
                format_command_result(param.second);
            }
        }

        _current_result_message_index = 0;

        _state = states::resume_processing;
    }

    void console::on_post_process_command() {
        const auto& result = _command_result_queue.front();

        auto command_action_msg = result.find_code("command_action");
        if (command_action_msg != nullptr) {
            auto params = command_action_msg->params();
            auto action_it = params.find("action");
            if (action_it != params.end()) {
                auto command = boost::get<std::string>(action_it->second);
                auto handler_it = _handlers.find(command);
                if (handler_it != _handlers.end()) {
                    if (handler_it->second(*this, params)) {
                        caret_down();
                        caret_home();
                    }
                }
            }
        } else {
            caret_down();
            caret_home();
        }

        write_message("Ready.");

        _command_result_queue.pop_front();

        _state = states::input;
    }

    void console::on_resume_process_command() {
        _remaining_lines = static_cast<int16_t>(_metrics.page_height - 2);

        _state = states::processing;
    }

    bool console::on_process_event(const SDL_Event* e) {
        auto ctrl_pressed = (SDL_GetModState() & KMOD_CTRL) != 0;
        auto shift_pressed = (SDL_GetModState() & KMOD_SHIFT) != 0;
        auto mode = _caret.mode();

        if (_state == states::processing) {
            return false;
        } else if (_state == states::wait) {
            if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_SPACE) {
                _state = states::resume_processing;
                return true;
            }
            return false;
        }

        if (e->type == SDL_TEXTINPUT) {
            if (mode == core::caret::mode::insert) {
                _document.shift_right(_caret.row(), _caret.column());
            }
            const char* c = &e->text.text[0];
            while (*c != '\0') {
                _document.put(
                        _caret.row(),
                        _caret.column(),
                        core::element_t {static_cast<uint8_t>(*c), core::attr_t{_color}});
                caret_right();
                c++;
            }
        } else if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_1: {
                    if (ctrl_pressed) {
                        core::attr_t a {};
                        auto it = _code_mapper.find("black");
                        if (it != _code_mapper.end()) {
                            it->second(a);
                            _color = a.color;
                            return true;
                        }
                    }
                    break;
                }
                case SDLK_2: {
                    if (ctrl_pressed) {
                        core::attr_t a {};
                        auto it = _code_mapper.find("white");
                        if (it != _code_mapper.end()) {
                            it->second(a);
                            _color = a.color;
                            return true;
                        }
                    }
                    break;
                }
                case SDLK_3: {
                    if (ctrl_pressed) {
                        core::attr_t a {};
                        auto it = _code_mapper.find("red");
                        if (it != _code_mapper.end()) {
                            it->second(a);
                            _color = a.color;
                            return true;
                        }
                    }
                    break;
                }
                case SDLK_4: {
                    if (ctrl_pressed) {
                        core::attr_t a {};
                        auto it = _code_mapper.find("cyan");
                        if (it != _code_mapper.end()) {
                            it->second(a);
                            _color = a.color;
                            return true;
                        }
                    }
                    break;
                }
                case SDLK_5: {
                    if (ctrl_pressed) {
                        core::attr_t a {};
                        auto it = _code_mapper.find("purple");
                        if (it != _code_mapper.end()) {
                            it->second(a);
                            _color = a.color;
                            return true;
                        }
                    }
                    break;
                }
                case SDLK_6: {
                    if (ctrl_pressed) {
                        core::attr_t a {};
                        auto it = _code_mapper.find("green");
                        if (it != _code_mapper.end()) {
                            it->second(a);
                            _color = a.color;
                            return true;
                        }
                    }
                    break;
                }
                case SDLK_7: {
                    if (ctrl_pressed) {
                        core::attr_t a {};
                        auto it = _code_mapper.find("blue");
                        if (it != _code_mapper.end()) {
                            it->second(a);
                            _color = a.color;
                            return true;
                        }
                    }
                    break;
                }
                case SDLK_8: {
                    if (ctrl_pressed) {
                        core::attr_t a {};
                        auto it = _code_mapper.find("yellow");
                        if (it != _code_mapper.end()) {
                            it->second(a);
                            _color = a.color;
                            return true;
                        }
                    }
                    break;
                }
                case SDLK_9: {
                    break;
                }
                case SDLK_ESCAPE: {
                    return transition_to("source_editor", {});
                }
                case SDLK_RETURN: {
                    if (!shift_pressed) {
                        core::result result;
                        auto str = find_command_string();
                        if (str.length() > 0 && _execute_command_callback != nullptr) {
                            _execute_command_callback(result, str);
                            _command_result_queue.push_back(result);
                            _state = states::pre_processing;
                            return true;
                        }
                    }

                    caret_down();
                    caret_home();

                    return true;
                }
                case SDLK_DELETE:
                    _document.shift_left(_caret.row(), _caret.column());
                    return true;

                case SDLK_BACKSPACE: {
                    if (caret_left())
                        caret_left();
                    _document.shift_left(_caret.row(), _caret.column());
                    return true;
                }
                case SDLK_UP:
                    caret_up();
                    return true;

                case SDLK_DOWN:
                    caret_down();
                    return true;

                case SDLK_LEFT:
                    caret_left();
                    return true;

                case SDLK_RIGHT:
                    caret_right();
                    return true;

                case SDLK_HOME:
                    if (ctrl_pressed) {
                        _caret.row(0);
                    }
                    caret_home();
                    return true;

                case SDLK_END:
                    if (ctrl_pressed) {
                        _caret.row(static_cast<uint8_t>(_metrics.page_height - 1));
                    }
                    caret_end();
                    return true;

                case SDLK_INSERT:
                    if (mode == core::caret::mode::insert)
                        _caret.overwrite();
                    else
                        _caret.insert();
                    return true;
            }
        }
        return false;
    }

    uint32_t console::write_message(
            const core::formatted_text_t& formatted_text,
            bool last_newline) {
        uint32_t line_count = 0;

        core::attr_t attr {
            _color,
            core::font::styles::normal,
            core::font::flags::none
        };

        caret_home();

        for (const auto& span : formatted_text.spans) {
            if (span.attr_code == "newline") {
                caret_down();
                caret_home();
                ++line_count;
                continue;
            }

            if (!span.attr_code.empty()) {
                auto it = _code_mapper.find(span.attr_code);
                if (it == _code_mapper.end()) {
                    attr.color = _color;
                    attr.flags = core::font::flags::none;
                    attr.style = core::font::styles::normal;
                } else {
                    it->second(attr);
                }
            }

            auto token = span.text.begin();
            while (token != span.text.end()) {
                _document.put(
                        _caret.row(),
                        _caret.column(),
                        core::element_t {static_cast<uint8_t>(*token), attr});
                caret_right();
                ++token;
            }
        }

        if (last_newline) {
            caret_down();
            caret_home();
            line_count++;
        }

        return line_count;
    }

    uint32_t console::write_message(
            const std::string& message,
            bool last_newline) {
        return write_message(
                core::text_formatter::format_text(message),
                last_newline);
    }

    void console::on_execute_command(const execute_command_callable& callable) {
        _execute_command_callback = callable;
    }

    void console::on_transition(const core::state_transition_callable& callable) {
        _transition_to_callback = callable;
    }

    void console::on_caret_changed(const console::caret_changed_callable& callable) {
        _caret_changed_callback = callable;
    }

    bool console::transition_to(const std::string& name, const core::parameter_dict& params) {
        bool consumed = false;
        if (_transition_to_callback) {
            consumed = _transition_to_callback(name, params);
        }
        return consumed;
    }

}