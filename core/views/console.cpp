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
#include <core/joysticks.h>
#include <common/string_support.h>
#include "console.h"

namespace ryu::core {

    console::command_action_dict console::s_handlers = {
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
            "list_machine",
            [](core::console& console, auto params) {
                console.transition_to("list_machine", params);
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
                            last_line.spans.push_back({"newline", ""});
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
                const auto& line = lines[line_index++];
                output_result.line_count = c->write_message(line);
            }
        }

        output_result.more_to_process = msg_index < messages.size()
                                        || line_index < lines.size();

        return output_result;
    }

    ///////////////////////////////////////////////////////////////////////////

    console::console(
            const std::string& name,
            core::view_host* host) : core::view(core::view::types::container, name, host),
                                               _caret("console-caret", host) {
    }

    console::~console() {
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

    void console::caret_end() {
        _document.line_end();
    }

    void console::first_page() {
        _document.first_page();
    }

    void console::caret_home() {
        _caret.column(0);
        _document.home();
    }

    bool console::more() const {
        return _more;
    }

    void console::define_actions() {
        auto caret_left_action = core::input_action::create(
                "console_caret_left",
                "IDE::Console",
                "Move the caret left on a line.");
        if (!caret_left_action->has_bindings()) {
            caret_left_action->bind_keys({core::key_left});
        }

        auto caret_right_action = core::input_action::create(
                "console_caret_right",
                "IDE::Console",
                "Move the caret right on a line.");
        if (!caret_right_action->has_bindings()) {
            caret_right_action->bind_keys({core::key_right});
        }

        auto caret_down_action = core::input_action::create(
                "console_caret_down",
                "IDE::Console",
                "Move the caret down a line.");
        if (!caret_down_action->has_bindings()) {
            caret_down_action->bind_keys({core::key_down});
        }

        auto caret_up_action = core::input_action::create(
                "console_caret_up",
                "IDE::Console",
                "Move the caret up a line.");
        if (!caret_up_action->has_bindings()) {
            caret_up_action->bind_keys({core::key_up});
        }

        auto page_up_action = core::input_action::create(
                "console_page_up",
                "IDE::Console",
                "Move up one page.");
        if (!page_up_action->has_bindings()) {
            page_up_action->bind_keys({core::key_page_up});
        }

        auto first_page_action = core::input_action::create(
                "console_first_page",
                "IDE::Console",
                "Move to the first page.");
        if (!first_page_action->has_bindings()) {
            first_page_action->bind_keys({core::mod_ctrl, core::key_page_up});
        }

        auto page_down_action = core::input_action::create(
                "console_page_down",
                "IDE::Console",
                "Move down one page.");
        if (!page_down_action->has_bindings()) {
            page_down_action->bind_keys({core::key_page_down});
        }

        auto last_page_action = core::input_action::create(
                "console_last_page",
                "IDE::Console",
                "Move to the last page.");
        if (!last_page_action->has_bindings()) {
            last_page_action->bind_keys({core::mod_ctrl, core::key_page_down});
        }

        auto caret_home_action = core::input_action::create(
                "console_caret_home",
                "IDE::Console",
                "Move caret to home position on line.");
        if (!caret_home_action->has_bindings()) {
            caret_home_action->bind_keys({core::key_home});
        }

        auto caret_end_action = core::input_action::create(
                "console_caret_end",
                "IDE::Console",
                "Move caret to end position on line.");
        if (!caret_end_action->has_bindings()) {
            caret_end_action->bind_keys({core::key_end});
        }

        auto delete_action = core::input_action::create(
                "console_delete",
                "IDE::Console",
                "Shift line left at caret position.");
        if (!delete_action->has_bindings()) {
            delete_action->bind_keys({core::key_delete});
        }

        auto delete_eol_action = core::input_action::create(
                "console_delete_eol",
                "IDE::Console",
                "Delete from caret to end of line.");
        if (!delete_eol_action->has_bindings()) {
            delete_eol_action->bind_keys({core::mod_ctrl, core::key_delete});
        }

        auto backspace_action = core::input_action::create(
                "console_backspace",
                "IDE::Console",
                "Move the caret left and then shift the line left.");
        if (!backspace_action->has_bindings()) {
            backspace_action->bind_keys({core::key_backspace});
        }

        auto insert_action = core::input_action::create(
                "console_insert",
                "IDE::Console",
                "Toggle insert/overwrite mode.");
        if (!insert_action->has_bindings()) {
            insert_action->bind_keys({core::key_insert});
        }

        auto insert_space_action = core::input_action::create(
                "console_insert_space",
                "IDE::Console",
                "Insert space at the caret position.");
        if (!insert_space_action->has_bindings()) {
            insert_space_action->bind_keys({core::mod_ctrl, core::key_insert});
        }

        auto soft_return_action = core::input_action::create(
                "console_soft_return",
                "IDE::Console",
                "Move caret to next line without executing any commands.");
        if (!soft_return_action->has_bindings()) {
            soft_return_action->bind_keys({core::mod_shift, core::key_return});
        }

        auto return_action = core::input_action::create(
                "console_return",
                "IDE::Console",
                "Execute any valid commands on the current line and move caret to next line.");
        if (!return_action->has_bindings()) {
            return_action->bind_keys({core::key_return});
        }

        auto resume_action = core::input_action::create(
                "console_resume",
                "IDE::Console",
                "Resume console output if in wait state.");
        if (!resume_action->has_bindings()) {
            resume_action->bind_keys({core::key_space});
        }

        auto text_input_action = core::input_action::create_no_map(
                "console_text_input",
                "IDE::Console",
                "Any ASCII text input (non-mappable).");
        if (!text_input_action->has_bindings()) {
            text_input_action->bind_text_input();
        }
    }

    void console::bind_events() {
        action_provider().register_handler(
                core::input_action::find_by_name("console_caret_left"),
                [this](const core::event_data_t& data) {
                    caret_left();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("console_caret_right"),
                [this](const core::event_data_t& data) {
                    caret_right();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("console_caret_down"),
                [this](const core::event_data_t& data) {
                    caret_down();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("console_caret_up"),
                [this](const core::event_data_t& data) {
                    caret_up();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("console_page_up"),
                [this](const core::event_data_t& data) {
                    page_up();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("console_first_page"),
                [this](const core::event_data_t& data) {
                    first_page();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("console_page_down"),
                [this](const core::event_data_t& data) {
                    page_down();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("console_last_page"),
                [this](const core::event_data_t& data) {
                    last_page();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("console_caret_home"),
                [this](const core::event_data_t& data) {
                    caret_home();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("console_caret_end"),
                [this](const core::event_data_t& data) {
                    caret_end();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("console_delete"),
            [this](const core::event_data_t& data) {
                _document.shift_line_left();
                return true;
            });
        action_provider().register_handler(
                core::input_action::find_by_name("console_delete_eol"),
                [this](const core::event_data_t& data) {
                    _document.shift_line_left(_metrics.page_width);
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("console_backspace"),
                [this](const core::event_data_t& data) {
                    if (caret_left())
                        caret_left();
                    _document.shift_line_left();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("console_insert"),
                [this](const core::event_data_t& data) {
                    if (_caret.mode() == core::caret::mode::insert)
                        _caret.overwrite();
                    else
                        _caret.insert();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("console_insert_space"),
                [this](const core::event_data_t& data) {
                    _document.shift_line_right();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("console_soft_return"),
                [this](const core::event_data_t& data) {
                    caret_newline();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("console_return"),
                [this](const core::event_data_t& data) {
                    if (_state != states::input)
                        return false;

                    core::result result;
                    auto str = find_command_string();
                    if (str.length() > 0 && _execute_command_callback != nullptr) {
                        _execute_command_callback(result, str);
                        _output_queue.emplace_back(result);
                        more(result.find_code("pipe_to_more") != nullptr);
                        _state = states::pre_processing;
                    }
                    caret_newline();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("console_resume"),
                [this](const core::event_data_t& data) {
                    if (_state != states::wait)
                        return false;
                    caret_home();
                    caret_up();
                    caret_up();
                    _state = states::resume_processing;
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("console_text_input"),
                [this](const core::event_data_t& data) {
                    if (data.c == core::ascii_escape)
                        return true;

                    if (_caret.mode() == core::caret::mode::insert)
                        _document.shift_line_right();

                    if (data.c == core::ascii_return) {
                        caret_newline();
                        return true;
                    }

                    // XXX: need to make tab stops configurable
                    if (data.c == core::ascii_tab) {
                        auto spaces = static_cast<uint8_t>(4 - (_document.virtual_column() % 4));
                        _document.shift_line_right(spaces);
                        caret_right(spaces);
                        return true;
                    }

                    _document.put(core::element_t {
                        static_cast<uint8_t>(data.c),
                        core::attr_t{_color}});

                    caret_right();

                    return true;
                });
    }

    void console::caret_up_line() {
        caret_up();
        caret_line_end();
    }

    void console::caret_newline() {
        caret_down();
        caret_home();
    }

    void console::caret_line_end() {
        _caret.column(_metrics.page_width);
        _document.end(_metrics.page_width);
    }

    void console::more(bool flag) {
        _more = flag;
    }

    void console::on_initialize() {
        _color = fg_color();

        _caret.initialize();
        _caret.overwrite();
        _caret.position(0, 0);
        _caret.palette(palette());
        _caret.font_family(font_family());
        _caret.on_caret_changed([&]() {
            raise_caret_changed();
        });

        auto& default_attr = _document.default_attr();
        default_attr.color = _color;
        default_attr.flags = core::font::flags::none;
        default_attr.style = core::font::styles::normal;

        _document.caret(&_caret);
        _document.document_size(4096, 128);
        _document.on_document_changed([&]() {
            raise_caret_changed();
        });
        _document.clear();

        add_child(&_caret);
        margin({_metrics.left_padding, _metrics.right_padding, 5, 5});

        define_actions();
        bind_events();
    }

    void console::raise_caret_changed() {
        if (_caret_changed_callback != nullptr)
            _caret_changed_callback(_caret, _document);
    }

    void console::caret_up(uint8_t rows) {
        if (_caret.up(rows))
            _document.scroll_up();
    }

    void console::caret_down(uint8_t rows) {
        if (_caret.down(rows))
            _document.scroll_down();
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
            if (_document.scroll_left())
                caret_up_line();
            overflow = true;
        }
        return overflow;
    }

    bool console::caret_right(uint8_t columns) {
        auto overflow = false;
        if (_caret.right(columns)) {
            if (_document.scroll_right())
                caret_newline();
            overflow = true;
        }
        return overflow;
    }

    // XXX: need to put CTRL-0 - CTRL-9 back in but
    //      should they map to names or palette indexes?
    void console::select_color(const std::string& name) {
        core::attr_t a {};
        auto it = _code_mapper.find(name);
        if (it != _code_mapper.end()) {
            it->second(a);
            _color = a.color;
        }
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

        auto current_palette = palette();
        if (current_palette == nullptr)
            return;

        auto y = bounds.top();
        auto row_index = _document.row();
        for (uint32_t row = 0; row < _metrics.page_height; row++) {
            uint16_t col_start = static_cast<uint16_t>(_document.column());
            uint16_t col_end = col_start + _metrics.page_width;

            auto chunks = _document.get_line_chunks(
                    static_cast<uint32_t>(row_index++),
                    col_start,
                    col_end);

            auto x = bounds.left();
            auto max_line_height = font_face()->line_height;

            for (const auto& chunk : chunks) {
                font_style(chunk.attr.style);

                auto face = font_face();
                if (face->line_height > max_line_height)
                    max_line_height = face->line_height;

                auto width = face->measure_text(chunk.text);
                auto color = current_palette->get(chunk.attr.color);

                if ((chunk.attr.flags & core::font::flags::reverse) != 0) {
                    surface.set_color(color);
                    color = current_palette->get(bg_color());
                    surface.fill_rect(core::rect{x, y, width, face->line_height});
                }

                surface.draw_text(face, x, y, chunk.text, color);
                x += width;
            }

            y += max_line_height;
        }
    }

    std::string console::find_command_string() {
        std::stringstream cmd;

        // first, seek backwards to a null value
        while (true) {
            caret_left();

            if (_caret.row() == 0
            &&  _document.row() == 0
            &&  _caret.column() == 0
            &&  _document.column() == 0) {
                break;
            }

            auto element = _document.get();
            if (element == nullptr || element->value == 0)
                break;
        }

        // then, if we're not in the upper left of the
        // document, move right by one
        if (!(_caret.row() == 0
        &&    _document.row() == 0
        &&    _caret.column() == 0
        &&    _document.column() == 0)) {
            caret_right();
        }

        // finally, scan forward to the next
        // null value to build the command or end of the page
        while (true) {
            if (_caret.row() == _metrics.page_height
            &&  _caret.column() == _metrics.page_width) {
                break;
            }

            auto element = _document.get();
            if (element == nullptr || element->value == 0)
                break;
            cmd << element->value;

            caret_right();
        }

        // ...and, if we got something, try to execute it
        return cmd.str();
    }

    void console::on_process_command() {
        auto& entry = _output_queue.front();

        auto max_item_count = 8;
        auto more_to_process = true;
        while (_remaining_lines > 0 && max_item_count > 0) {
            auto output_result = entry.process(this);
            more_to_process = output_result.more_to_process;
            if (!more_to_process)
                break;
            _remaining_lines -= output_result.line_count;
            --max_item_count;
        }

        if (more_to_process) {
            if (more()) {
                caret_newline();
                write_message("<rev><bold> MORE (SPACE to continue) <>", false);
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
        header_line.spans.push_back({"rev", " "});
        header_line.spans.push_back({"bold", ""});
        for (size_t i = 0; i < table.headers.size(); i++) {
            auto& col = table.headers[i];
            auto column_pad = i < table.headers.size() - 1 ?
                              col.padding : 0;

            header_line.spans.push_back({
                "",
                fmt::format(
                        get_alignment_format(col.alignment),
                        col.text.substr(0, col.width),
                        col.width + column_pad)
            });
        }
        header_line.spans.push_back({"", " "});
        list.push_back(header_line);

        // XXX: need to implement truncation for styled_text
        for (size_t i = 0; i < table.rows.size() - 1; i++) {
            const auto& row = table.rows[i];
            core::formatted_text_t row_line{};
            row_line.spans.push_back({"", " "});

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
                                styled_text,
                                header.width);
                            break;
                        case alignment::horizontal::right:
                            formatted_text = text_formatter::format_text_right_padded(
                                styled_text,
                                header.width);
                            break;
                        case alignment::horizontal::center:
                            break;
                    }
                    for (const auto& span : formatted_text.spans)
                        row_line.spans.push_back(span);
                    if (column_pad > 0) {
                        row_line.spans.push_back({
                            "",
                            std::string(column_pad, ' ')
                        });
                    }
                } else {
                    row_line.spans.push_back({
                         "",
                         fmt::format(
                                 get_alignment_format(header.alignment),
                                 col.substr(0, header.width),
                                 header.width + column_pad)
                    });
                }

                total_width += header.width + column_pad;
            }

            list.push_back(row_line);
            for (uint8_t k = 0; k < table.line_spacing; k++) {
                list.push_back({});
            }
        }

        core::formatted_text_t footer_line {};
        footer_line.spans.push_back({"bold", " "});
        const auto& footer_data_row = table.rows[table.rows.size() - 1];
        for (size_t i = 0; i < footer_data_row.columns.size(); i++) {
            auto& footer = table.footers[i];
            const auto& col = footer_data_row.columns[i];
            auto column_pad = i < footer_data_row.columns.size() - 1 ?
                              footer.padding : 0;

            footer_line.spans.push_back({
                "",
                fmt::format(
                        get_alignment_format(footer.alignment),
                        col.substr(0, footer.width),
                        footer.width + column_pad)
            });
        }
        list.push_back(footer_line);
    }

    // XXX: right aligned columns seem to ignore padding?
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
    }

    void console::on_pre_process_command() {
        _state = states::resume_processing;
    }

    void console::on_post_process_command() {
        const auto& entry = _output_queue.front();

        auto command_action_msg = entry.result.find_code("command_action");
        if (command_action_msg != nullptr) {
            auto command = command_action_msg->get_parameter<std::string>("action");
            auto handler_it = s_handlers.find(command);
            if (handler_it != s_handlers.end()) {
                handler_it->second(*this, command_action_msg->params());
            }
        }

        caret_newline();
        write_message("Ready.");

        _output_queue.pop_front();

        more(false);

        _state = states::input;
    }

    void console::on_resume_process_command() {
        _remaining_lines = static_cast<int16_t>(_metrics.page_height - 2);

        _state = states::processing;
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
                caret_newline();
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

            for (size_t i = 0; i < span.text.length(); i++) {
                _document.put(core::element_t {
                    static_cast<uint8_t>(span.text[i]),
                    attr});
                caret_right();
            }
        }

        if (last_newline) {
            caret_newline();
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

    void console::on_update(uint32_t, core::pending_event_list& events) {
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