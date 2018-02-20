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
#include <common/string_support.h>
#include "console.h"

namespace ryu::core {

    console::command_action_dict console::_handlers = {
        {
            "clear",
            [](core::console& console, auto params) {
                console._document.clear();
                console._caret.row(0);
                console.caret_home();
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

    void console::page_up() {
        _document.page_up();
    }

    void console::page_down() {
        _document.page_down();
    }

    void console::last_page() {
        _document.last_page();
    }

    void console::first_page() {
        _document.first_page();
    }

    void console::caret_end() {
        _caret.column(_metrics.page_width);
        _document.end(_metrics.page_width);
    }

    void console::caret_home() {
        _caret.column(0);
        _document.home();
    }

    void console::initialize() {
        _attr.color = fg_color();

        _caret.overwrite();
        _caret.initialize(0, 0);
        _caret.palette(palette());
        _caret.font_family(font_family());
        _caret.on_caret_changed([&]() {
            raise_caret_changed();
        });

        _document.caret(&_caret);
        _document.document_size(4096, 128);
        _document.on_document_changed([&]() {
            raise_caret_changed();
        });
        _document.clear();

        add_child(&_caret);
        margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
    }

    bool console::more() const {
        return _more;
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

    void console::more(bool flag) {
        _more = flag;
    }

    void console::caret_newline() {
        caret_down();
        caret_home();
    }

    void console::raise_caret_changed() {
        if (_caret_changed_callback != nullptr)
            _caret_changed_callback(_caret, _document);
    }

    void console::caret_up(uint8_t rows) {
        if (_caret.up(rows)) {
            _document.scroll_up();
        }
    }

    void console::caret_down(uint8_t rows) {
        if (_caret.down(rows)) {
            _document.scroll_down();
        }
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
        auto overflow = false;
        if (_caret.left(columns)) {
            if (_document.scroll_left()) {
                caret_up();
                caret_end();
            }
            overflow = true;
        }
        return overflow;
    }

    bool console::caret_right(uint8_t columns) {
        auto overflow = false;
        if (_caret.right(columns)) {
            if (_document.scroll_right()) {
                caret_newline();
            }
            overflow = true;
        }
        return overflow;
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

        auto y = bounds.top();
        auto max_line_height = font_face()->line_height;
        uint32_t row = static_cast<uint32_t>(_document.row());
        uint32_t column_start = static_cast<uint32_t>(_document.column());
        uint32_t column_end = column_start + _metrics.page_width;

        for (auto line = 0; line < _metrics.page_height; line++) {
            auto spans = _document.line_at(row, column_start, column_end);

            auto x = bounds.left();
            for (const auto& span : spans) {
                font_style(span.attr.style);

                auto face = font_face();
                if (face->line_height > max_line_height)
                    max_line_height = face->line_height;

                auto width = face->measure_text(span.text);
                auto color = pal[span.attr.color];

                if ((span.attr.flags & core::font::flags::reverse) != 0) {
                    surface.set_color(color);
                    color = pal[bg_color()];
                    surface.fill_rect(core::rect{x, y, width, face->line_height});
                }

                surface.draw_text(face, x, y, span.text, color);
                x += width;
            }

            y += max_line_height;
            ++row;
        }
    }

    std::string console::find_command_string() {
        std::stringstream cmd;

        // first, seek backwards to a null value
        while (true) {
            caret_left();
            if (_caret.row() == 0 && _caret.column() == 0)
                break;
            element_t element {};
            if (!_document.get(element))
                break;
        }

        // second, scan forward to the next null value to build the command
        while (true) {
            caret_right();
            element_t element {};
            if (!_document.get(element))
                break;
            cmd << element.value;
        }

        // finally, if we got something, try to execute it
        return cmd.str();
    }

    void console::on_process_command() {
        auto& entry = _output_queue.front();

        auto more_to_process = true;
        while (_remaining_lines > 0) {
            auto output_result = entry.process(this);
            more_to_process = output_result.more_to_process;
            if (!more_to_process)
                break;
            _remaining_lines -= output_result.line_count;
        }

        if (more_to_process) {
            if (more()) {
                caret_newline();
                write_message("<rev><bold> MORE (SPACE to continue) <>");
                _state = states::wait;
            } else {
                _state = states::resume_processing;
            }
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
        auto target_width = _document.columns() - 10;

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

    void console::format_data_table(
            formatted_text_list& list,
            data_table_t& table) {
        using format_options = core::data_table_column_t::format_options;

        scale_columns(table.headers);
        scale_columns(table.footers);

        core::formatted_text_t header_line {};
        header_line.spans.push_back(text_formatter::span_for_code(_code_mapper, _attr, "rev", " "));
        header_line.spans.push_back(text_formatter::span_for_code(_code_mapper, _attr, "bold", " "));
        for (size_t i = 0; i < table.headers.size(); i++) {
            auto& col = table.headers[i];
            auto column_pad = i < table.headers.size() - 1 ?
                              col.padding : 0;

            header_line.spans.push_back(text_formatter::span_for_code(
                    _code_mapper,
                    _attr,
                    "",
                    fmt::format(
                            get_alignment_format(col.alignment),
                            col.text.substr(0, col.width),
                            col.width + column_pad)));
        }
        header_line.spans.push_back(text_formatter::span_for_code(
                _code_mapper,
                _attr,
                "reset",
                " \n"));
        list.push_back(header_line);

        // XXX: need to implement truncation for styled_text
        for (size_t i = 0; i < table.rows.size() - 1; i++) {
            const auto& row = table.rows[i];
            core::formatted_text_t row_line{};
            row_line.add_empty_span(_code_mapper, _attr);

            size_t total_width = 0;
            for (size_t j = 0; j < row.columns.size(); j++) {
                const auto& header = table.headers[j];
                const auto& col = row.columns[j];
                auto column_pad = static_cast<uint32_t>(j < row.columns.size() - 1 ?
                                  header.padding : 0);

                std::string styled_text;
                auto styled = (header.options & format_options::style_codes) != 0;
                auto word_wrapped = (header.options & format_options::word_wrap) != 0;

                if (word_wrapped) {
                    styled_text = word_wrap(
                            col,
                            header.width,
                            total_width + 1);
                    styled = true;
                } else {
                    styled_text = col;
                }

                if (styled) {
                    core::formatted_text_t formatted_text;
                    switch (header.alignment) {
                        case alignment::horizontal::none:
                        case alignment::horizontal::left:
                            formatted_text = text_formatter::format_text_left_padded(
                                    _code_mapper,
                                    _attr,
                                    styled_text,
                                    header.width);
                            break;
                        case alignment::horizontal::right:
                            formatted_text = text_formatter::format_text_right_padded(
                                    _code_mapper,
                                    _attr,
                                    styled_text,
                                    header.width);
                            break;
                        case alignment::horizontal::center:
                            break;
                    }
                    for (const auto& span : formatted_text.spans)
                        row_line.spans.push_back(span);
                    if (column_pad > 0) {
                        row_line.spans.push_back(text_formatter::span_for_code(
                                _code_mapper,
                                _attr,
                                "",
                                std::string(column_pad, ' ')));
                    }
                } else {
                    row_line.spans.push_back(text_formatter::span_for_code(
                            _code_mapper,
                            _attr,
                            "",
                            fmt::format(
                                    get_alignment_format(header.alignment),
                                    col.substr(0, header.width),
                                    header.width + column_pad)));
                }

                total_width += header.width + column_pad;
            }

            row_line.spans.push_back(text_formatter::span_for_code(
                    _code_mapper,
                    _attr,
                    "reset",
                    "\n"));

            list.push_back(row_line);

            for (uint8_t k = 0; k < table.line_spacing; k++) {
                formatted_text_t spacing_line {};
                spacing_line.spans.push_back(text_formatter::span_for_code(
                        _code_mapper,
                        _attr,
                        "reset",
                        "\n"));
                list.push_back(spacing_line);
            }
        }

        core::formatted_text_t footer_line {};
        footer_line.spans.push_back(text_formatter::span_for_code(_code_mapper, _attr, "bold", " "));
        const auto& footer_data_row = table.rows[table.rows.size() - 1];
        for (size_t i = 0; i < footer_data_row.columns.size(); i++) {
            auto& footer = table.footers[i];
            const auto& col = footer_data_row.columns[i];
            auto column_pad = i < footer_data_row.columns.size() - 1 ?
                              footer.padding : 0;

            footer_line.spans.push_back(text_formatter::span_for_code(
                    _code_mapper,
                    _attr,
                    "",
                    fmt::format(
                            get_alignment_format(footer.alignment),
                            col.substr(0, footer.width),
                            footer.width + column_pad)));
        }
        footer_line.spans.push_back(text_formatter::span_for_code(
                _code_mapper,
                _attr,
                "reset",
                "\n"));
        list.push_back(footer_line);
    }

    void console::format_command_result(
            const parameter_variant_t& param,
            formatted_text_list& lines) {
        switch (param.which()) {
            case parameter_dict_types::table: {
                auto table = boost::get<core::data_table_t>(param);
                format_data_table(lines, table);
                break;
            }
            case parameter_dict_types::string: {
                auto value = boost::get<std::string>(param);
                core::formatted_text_t line {};
                line.spans.push_back(text_formatter::span_for_code(_code_mapper, _attr, "",  value));
                lines.push_back(line);
                break;
            }
            case parameter_dict_types::integer32: {
                auto value = boost::get<uint32_t>(param);
                core::formatted_text_t line {};
                line.spans.push_back(text_formatter::span_for_code(_code_mapper, _attr, "", std::to_string(value)));
                lines.push_back(line);
                break;
            }
            case parameter_dict_types::boolean: {
                auto value = boost::get<bool>(param);
                core::formatted_text_t line {};
                line.spans.push_back(text_formatter::span_for_code(_code_mapper, _attr, "", std::to_string(value)));
                lines.push_back(line);
                break;
            }
            default: {
                break;
            }
        }
    }

    void console::on_pre_process_command() {
        _state = states::resume_processing;
    }

    void console::on_post_process_command() {
        const auto& entry = _output_queue.front();

        auto command_action_msg = entry.result.find_code("command_action");
        if (command_action_msg != nullptr) {
            auto command = command_action_msg->get_parameter<std::string>("action");
            auto handler_it = _handlers.find(command);
            if (handler_it != _handlers.end()) {
                handler_it->second(*this, command_action_msg->params());
            }
        }

        caret_newline();
        write_message("Ready.");
        caret_newline();

        _output_queue.pop_front();

        more(false);

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

        if (_state == states::processing
        ||  _state == states::post_processing
        ||  _state == states::resume_processing) {
            return false;
        } else if (_state == states::wait) {
            if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_SPACE) {
                caret_home();
                caret_up();
                caret_up();
                _state = states::resume_processing;
                return true;
            }
            return false;
        }

        if (e->type == SDL_TEXTINPUT) {
            const char* c = &e->text.text[0];
            auto elements = element_list_t::from_string(_attr, c);
            _document.insert(elements);
            return true;
        } else if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_1: {
                    if (ctrl_pressed) {
                        core::attr_t a {};
                        auto it = _code_mapper.find("black");
                        if (it != _code_mapper.end()) {
                            it->second(a);
                            _attr.color = a.color;
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
                            _attr.color = a.color;
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
                            _attr.color = a.color;
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
                            _attr.color = a.color;
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
                            _attr.color = a.color;
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
                            _attr.color = a.color;
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
                            _attr.color = a.color;
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
                            _attr.color = a.color;
                            return true;
                        }
                    }
                    break;
                }
                case SDLK_9: {
                    break;
                }
                case SDLK_ESCAPE: {
                    break;
                }
                case SDLK_RETURN: {
                    if (!shift_pressed) {
                        core::result result;
                        auto str = find_command_string();
                        if (str.length() > 0 && _execute_command_callback != nullptr) {
                            _execute_command_callback(result, str);
                            _output_queue.emplace_back(result);
                            more(result.find_code("pipe_to_more") != nullptr);
                            _state = states::pre_processing;
                        }
                    }
                    caret_newline();
                    return true;
                }
                case SDLK_DELETE: {
                    if (ctrl_pressed) {
                        _document.shift_left(_metrics.page_width);
                    } else {
                        _document.shift_left();
                    }
                    return true;
                }
                case SDLK_BACKSPACE: {
                    if (caret_left())
                        caret_left();
                    _document.shift_left();
                    return true;
                }
                case SDLK_UP: {
                    caret_up();
                    return true;
                }
                case SDLK_DOWN: {
                    caret_down();
                    return true;
                }
                case SDLK_LEFT: {
                    caret_left();
                    return true;
                }
                case SDLK_RIGHT: {
                    caret_right();
                    return true;
                }
                case SDLK_PAGEUP: {
                    if (ctrl_pressed)
                        first_page();
                    else
                        page_up();
                    return true;
                }
                case SDLK_PAGEDOWN: {
                    if (ctrl_pressed)
                        last_page();
                    else
                        page_down();
                    return true;
                }
                case SDLK_HOME: {
                    if (ctrl_pressed) {
                        _caret.row(0);
                    }
                    caret_home();
                    return true;
                }
                case SDLK_END: {
                    if (ctrl_pressed) {
                        _caret.row(static_cast<uint8_t>(_metrics.page_height - 1));
                    }
                    caret_end();
                    return true;
                }
                case SDLK_INSERT: {
                    if (mode == core::caret::mode::insert)
                        _caret.overwrite();
                    else
                        _caret.insert();
                    return true;
                }
                default: {
                    break;
                }
            }
        }

        return false;
    }

    uint32_t console::write_message(const std::string& message) {
        auto formatted_text = core::text_formatter::format_text(
                _code_mapper,
                _attr,
                message);
        return write_message(formatted_text);
    }

    uint32_t console::write_message(core::formatted_text_t& formatted_text) {
        caret_home();
        auto elements = formatted_text.spans.sequence();
        _document.insert(elements);
        caret_newline();
        return 1;
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

    ///////////////////////////////////////////////////////////////////////////

    output_process_result_t output_queue_entry_t::process(console* c) {
        output_process_result_t output_result{};

        auto messages = result.messages();
        if (msg_index < messages.size()) {
            const auto& msg = messages[msg_index++];
            if (msg.type() == core::result_message::types::data) {
                if (msg.code() == "command_result") {
                    auto count = 0;
                    auto params = msg.params();
                    for (auto it = params.begin(); it != params.end(); ++it) {
                        c->format_command_result(it->second, lines);
                        if (!lines.empty() && count < params.size() - 1) {
                            auto& last_line = lines[lines.size() - 1];
                            last_line.spans.push_back(text_formatter::span_for_code(
                                    c->_code_mapper,
                                    c->_attr,
                                    "",
                                    "\n"));
                        }
                        ++count;
                    }
                }
            } else {
                auto error_part = msg.is_error() ?
                                  "<bold><red>ERROR:<> " :
                                  "";
                output_result.line_count = c->write_message(
                        fmt::format("{}{}", error_part, msg.message()));
            }
        } else {
            if (line_index < lines.size()) {
                auto& line = lines[line_index++];
                output_result.line_count = c->write_message(line);
            }
        }

        output_result.more_to_process = msg_index < messages.size()
                                        || line_index < lines.size();

        return output_result;
    }

}