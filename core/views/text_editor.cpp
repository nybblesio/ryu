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
#include <core/input_binding.h>
#include "text_editor.h"

namespace ryu::core {

    text_editor::text_editor(
            const std::string& name,
            core::view_host* host) : core::view(core::view::types::container, name, host),
                                               _caret("editor-caret", host) {
    }

    void text_editor::clear() {
        _document.clear();
        first_page();
        _caret.row(0);
        _caret.column(0);
        update_virtual_position();
    }

    void text_editor::page_up() {
        _document.page_up();
        update_virtual_position();
    }

    void text_editor::page_down() {
        _document.page_down();
        update_virtual_position();
    }

    void text_editor::scroll_up() {
        _document.scroll_up();
        update_virtual_position();
    }

    void text_editor::caret_end() {
        auto end_column = _document.find_line_end(_vrow);
        caret_home();
        for (auto i = 0; i < end_column; i++)
            caret_right();
        update_virtual_position();
    }

    void text_editor::last_page() {
        _document.last_page();
        update_virtual_position();
    }

    void text_editor::first_page() {
        _document.first_page();
        update_virtual_position();
    }

    void text_editor::caret_home() {
        _caret.column(0);
        _document.home();
        update_virtual_position();
    }

    void text_editor::scroll_down() {
        _document.scroll_down();
        update_virtual_position();
    }

    void text_editor::scroll_left() {
        _document.scroll_left();
        update_virtual_position();
    }

    void text_editor::bind_events() {
        auto caret_select_left_action = core::input_action::create(
            "text_editor_caret_select_left",
            "IDE::Text Editor",
            "Move the caret left on a line in selection mode.");
        caret_select_left_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                update_selection(_vcol);
                auto element = _document.get(_vrow, _vcol);
                if (element != nullptr)
                    element->attr.flags |= core::font::flags::reverse;
                caret_left();
                return true;
            });
        caret_select_left_action->bind_keys({core::mod_shift, core::key_left});

        auto caret_left_action = core::input_action::create(
            "text_editor_caret_left",
            "IDE::Text Editor",
            "Move the caret left on a line.");
        caret_left_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                end_selection();
                caret_left();
                return true;
            });
        caret_left_action->bind_keys({core::key_left});

        auto caret_select_right_action = core::input_action::create(
            "text_editor_caret_select_right",
            "IDE::Text Editor",
            "Move the caret right on a line in selection mode.");
        caret_select_right_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                update_selection(_vcol);
                auto element = _document.get(_vrow, _vcol);
                if (element != nullptr)
                    element->attr.flags |= core::font::flags::reverse;
                caret_right();
                return true;
            });
        caret_select_right_action->bind_keys({core::mod_shift, core::key_right});

        auto caret_right_action = core::input_action::create(
            "text_editor_caret_right",
            "IDE::Text Editor",
            "Move the caret right on a line.");
        caret_right_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                end_selection();
                caret_right();
                return true;
            });
        caret_right_action->bind_keys({core::key_right});

        auto caret_select_down_action = core::input_action::create(
            "text_editor_caret_select_down",
            "IDE::Text Editor",
            "Move the caret down a line in selection mode.");
        caret_select_down_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                auto line_end = _document.find_line_end(_vrow);
                update_selection(line_end);
                for (auto col = _vcol; col < line_end; col++) {
                    auto element = _document.get(_vrow, col);
                    if (element != nullptr)
                        element->attr.flags |= core::font::flags::reverse;
                }
                caret_newline();
                return true;
            });
        caret_select_down_action->bind_keys({core::mod_shift, core::key_down});

        auto caret_down_action = core::input_action::create(
            "text_editor_caret_down",
            "IDE::Text Editor",
            "Move the caret down a line.");
        caret_down_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                end_selection();
                caret_down();
                return true;
            });
        caret_down_action->bind_keys({core::key_down});

        auto caret_select_up_action = core::input_action::create(
            "text_editor_caret_select_up",
            "IDE::Text Editor",
            "Move the caret up a line in selection mode.");
        caret_select_up_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                auto line_end = _document.find_line_end(_vrow);
                update_selection(line_end);
                for (auto col = _vcol; col < line_end; col++) {
                    auto element = _document.get(_vrow, col);
                    if (element != nullptr)
                        element->attr.flags |= core::font::flags::reverse;
                }
                caret_up();
                caret_home();
                return true;
            });
        caret_select_up_action->bind_keys({core::mod_shift, core::key_up});

        auto caret_up_action = core::input_action::create(
            "text_editor_caret_up",
            "IDE::Text Editor",
            "Move the caret left up a line.");
        caret_up_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                end_selection();
                caret_up();
                return true;
            });
        caret_up_action->bind_keys({core::key_up});

        auto page_up_action = core::input_action::create(
            "text_editor_page_up",
            "IDE::Text Editor",
            "Move up one page.");
        page_up_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                page_up();
                return true;
            });
        page_up_action->bind_keys({core::key_page_up});

        auto page_down_action = core::input_action::create(
            "text_editor_page_down",
            "IDE::Text Editor",
            "Move down one page.");
        page_down_action->register_handler(
            core::action_sink::view,
            core::action_sink::default_filter,
            [this](const core::event_data_t& data) {
                page_down();
                return true;
            });
        page_down_action->bind_keys({core::key_page_down});

        auto insert_action = core::input_action::create(
            "text_editor_insert",
            "IDE::Text Editor",
            "Toggle insert/overwrite mode.");
        insert_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                if (_caret.mode() == core::caret::mode::insert)
                    _caret.overwrite();
                else
                    _caret.insert();
                return true;
            });
        insert_action->bind_keys({core::key_insert});

        auto copy_action = core::input_action::create(
            "text_editor_copy",
            "IDE::Text Editor",
            "Copy the active selection.");
        copy_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                std::stringstream stream;
                get_selected_text(stream);
                SDL_SetClipboardText(stream.str().c_str());
                return true;
            });
        copy_action->bind_keys({core::mod_ctrl, core::key_c});

        auto cut_action = core::input_action::create(
            "text_editor_cut",
            "IDE::Text Editor",
            "Cut the active selection.");
        cut_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                std::stringstream stream;
                get_selected_text(stream);
                SDL_SetClipboardText(stream.str().c_str());
                delete_selection();
                return true;
            });
        cut_action->bind_keys({core::mod_ctrl, core::key_x});

        auto paste_action = core::input_action::create(
            "text_editor_paste",
            "IDE::Text Editor",
            "Paste the clipboard at the caret position.");
        paste_action->register_handler(
            core::action_sink::view,
            [this](const core::event_data_t& data) {
                return focused() && SDL_HasClipboardText();
            },
            [this](const core::event_data_t& data) {
                auto text = SDL_GetClipboardText();
                insert_text(text);
                return true;
            });
        paste_action->bind_keys({core::mod_ctrl, core::key_v});

        auto return_action = core::input_action::create(
            "text_editor_return",
            "IDE::Text Editor",
            "Move caret to the next line at the home position.");
        return_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                if (_caret.mode() == core::caret::mode::insert)
                    _document.split_line(_vrow, _vcol);
                caret_newline();
                return true;
            });
        return_action->bind_keys({core::key_return});

        auto shift_tab_action = core::input_action::create(
            "text_editor_shift_tab",
            "IDE::Text Editor",
            "Move caret to the previous tab stop.");
        shift_tab_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                if (_caret.column() == 0)
                    return true;
                auto spaces = static_cast<uint8_t>(4 - (_vcol % 4));
                caret_left(spaces);
                _document.shift_line_left(_vrow, _vcol, spaces);
                return true;
            });
        shift_tab_action->bind_keys({core::mod_shift, core::key_tab});

        auto tab_action = core::input_action::create(
            "text_editor_tab",
            "IDE::Text Editor",
            "Move caret to the next tab stop.");
        tab_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                auto spaces = static_cast<uint8_t>(4 - (_vcol % 4));
                _document.shift_line_right(_vrow, _vcol, spaces);
                for (auto col = _vcol; col < _vcol + spaces; col++)
                    _document.put(_vrow, col, core::element_t {0, _document.default_attr()});
                caret_right(spaces);
                return true;
            });
        tab_action->bind_keys({core::key_tab});

        auto delete_action = core::input_action::create(
            "text_editor_delete",
            "IDE::Text Editor",
            "Shift the line left at the caret position.");
        delete_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                if (_selection.valid()) {
                    delete_selection();
                } else {
                    if (_document.is_line_empty(_vrow)) {
                        _document.delete_line(_vrow);
                    } else {
                        _document.shift_line_left(_vrow, _vcol);
                    }
                }
                return true;
            });
        delete_action->bind_keys({core::key_delete});

        auto backspace_action = core::input_action::create(
            "text_editor_backspace",
            "IDE::Text Editor",
            "Move the caret left and shift the line left.");
        backspace_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                if (_document.is_line_empty(_vrow)) {
                    _document.delete_line(_vrow);
                } else if (_caret.column() == 0) {
                    _document.delete_line(_vrow);
                    caret_up();
                } else {
                    caret_left();
                    _document.shift_line_left(_vrow, _vcol);
                }
                return true;
            });
        backspace_action->bind_keys({core::key_backspace});

        auto first_page_action = core::input_action::create(
            "text_editor_first_page",
            "IDE::Text Editor",
            "Move to the first page in the home position.");
        first_page_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                first_page();
                return true;
            });
        first_page_action->bind_keys({core::mod_ctrl, core::key_home});

        auto select_home_action = core::input_action::create(
            "text_editor_select_home",
            "IDE::Text Editor",
            "Move the caret to the home position in selection mode.");
        select_home_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                update_selection(_vcol);
                for (auto col = _vcol; col >= 0; col--) {
                    auto element = _document.get(_vrow, col);
                    if (element != nullptr)
                        element->attr.flags |= core::font::flags::reverse;
                }
                caret_home();
                return true;
            });
        select_home_action->bind_keys({core::mod_shift, core::key_home});

        auto home_action = core::input_action::create(
            "text_editor_home",
            "IDE::Text Editor",
            "Move the caret to the home position.");
        home_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                end_selection();
                caret_home();
                return true;
            });
        home_action->bind_keys({core::key_home});

        auto last_page_action = core::input_action::create(
            "text_editor_last_page",
            "IDE::Text Editor",
            "Move to the last page in the end position.");
        last_page_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                last_page();
                return true;
            });
        last_page_action->bind_keys({core::mod_ctrl, core::key_end});

        auto select_end_action = core::input_action::create(
            "text_editor_select_end",
            "IDE::Text Editor",
            "Move the caret to the end position in selection mode.");
        select_end_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                auto line_end = _document.find_line_end(_vrow);
                update_selection(line_end);
                for (auto col = _vcol; col < line_end; col++) {
                    auto element = _document.get(_vrow, col);
                    if (element != nullptr)
                        element->attr.flags |= core::font::flags::reverse;
                }
                caret_end();
                return true;
            });
        select_end_action->bind_keys({core::mod_shift, core::key_end});

        auto end_action = core::input_action::create(
            "text_editor_end",
            "IDE::Text Editor",
            "Move the caret to the end position.");
        end_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                end_selection();
                caret_end();
                return true;
            });
        end_action->bind_keys({core::key_end});

        auto text_input_action = core::input_action::create_no_map(
            "text_editor_input",
            "IDE::Console",
            "Any ASCII text input (non-mappable).");
        text_input_action->register_handler(
            core::action_sink::view,
            std::bind(&text_editor::input_event_filter, this, std::placeholders::_1),
            [this](const core::event_data_t& data) {
                if (_caret.mode() == core::caret::mode::insert)
                    _document.shift_line_right(_vrow, _vcol);
                _document.put(
                    _vrow,
                    _vcol,
                    core::element_t {static_cast<uint8_t>(data.c), _document.default_attr()});
                caret_right();
                return true;
            });
        text_input_action->bind_text_input();
    }

    void text_editor::caret_newline() {
        caret_down();
        caret_home();
    }

    bool text_editor::scroll_right() {
        bool clamped = _document.scroll_right();
        update_virtual_position();
        return clamped;
    }

    void text_editor::end_selection() {
        if (_caret.mode() != core::caret::mode::select)
            return;

        _caret.insert();

        for_each_selection_char([&](uint32_t row, uint16_t col) {
            auto element = _document.get(row, col);
            if (element != nullptr)
                element->attr.flags &= ~core::font::flags::reverse;
        });

        _selection.end(_vrow, _vcol);
    }

    void text_editor::on_focus_changed() {
        _caret.enabled(focused());
    }

    void text_editor::raise_caret_changed() {
        if (_caret_changed_callback != nullptr)
            _caret_changed_callback(_caret, _document);
    }

    void text_editor::caret_up(uint8_t rows) {
        if (_caret.up(rows))
            scroll_up();
        update_virtual_position();
    }

    void text_editor::goto_line(uint32_t row) {
        if (row < 1)
            row = 1;
        else if (row > _document.rows())
            row = _document.rows();
        _document.row(row - 1);
        update_virtual_position();
    }

    void text_editor::caret_down(uint8_t rows) {
        if (_caret.down(rows))
            scroll_down();
        update_virtual_position();
    }

    void text_editor::update_virtual_position() {
        _vrow = static_cast<uint32_t>(_document.row() + _caret.row());
        _vcol = static_cast<uint16_t>(_document.column() + _caret.column());

        if (_caret.mode() != core::caret::mode::select)
            _selection.clear();
    }

    void text_editor::caret_color(uint8_t value) {
        _caret.fg_color(value);
    }

    void text_editor::caret_left(uint8_t columns) {
        if (_caret.left(columns))
            scroll_left();
        update_virtual_position();
    }

    bool text_editor::caret_right(uint8_t columns) {
        auto clamped = false;
        if (_caret.right(columns)) {
            clamped = scroll_right();
            if (clamped)
                caret_newline();
        }
        update_virtual_position();
        return clamped;
    }

    void text_editor::selection_color(uint8_t value) {
        _selection_color = value;
    }

    void text_editor::find(const std::string& needle) {
    }

    void text_editor::line_number_color(uint8_t value) {
        _line_number_color = value;
    }

    void text_editor::update_selection(uint16_t line_end) {
        if (_caret.mode() == core::caret::mode::select) {
            if (_vrow < _selection.start().row && line_end < _selection.start().column)
                _selection.start(_vrow, line_end);
            else
                _selection.end(_vrow, line_end);
        } else {
            _caret.select();
            _selection.start(_vrow, 0);
            _selection.end(_vrow, line_end);
        }
    }

    void text_editor::on_draw(core::renderer& surface) {
        auto bounds = client_bounds();
        auto pal = *palette();

        auto& info_text_color = pal[_line_number_color];

        auto y = bounds.top();
        auto row_start = _document.row();
        auto row_stop = row_start + _metrics.page_height;
        for (auto row = row_start; row < row_stop; row++) {
            surface.draw_text(
                    font_face(),
                    bounds.left(),
                    y,
                    fmt::format("{0:04}", row + 1),
                    info_text_color);

            uint16_t col_start = static_cast<uint16_t>(_document.column());
            uint16_t col_end = col_start + _metrics.page_width;

            auto chunks = _document.get_line_chunks(
                    static_cast<uint32_t>(row),
                    col_start,
                    col_end);

            auto max_line_height = font_face()->line_height;
            auto x = bounds.left() + _caret.padding().left();

            for (const auto& chunk : chunks) {
                font_style(chunk.attr.style);

                auto face = font_face();
                if (face->line_height > max_line_height)
                    max_line_height = face->line_height;

                auto width = face->measure_text(chunk.text);
                auto color = pal[chunk.attr.color];

                if ((chunk.attr.flags & core::font::flags::reverse) != 0) {
                    surface.push_blend_mode(SDL_BLENDMODE_BLEND);
                    auto selection_color = pal[_selection_color];
                    selection_color.alpha(0x7f);
                    surface.set_color(selection_color);
                    surface.fill_rect(core::rect{x, y, width, face->line_height});
                    surface.pop_blend_mode();
                }

                surface.draw_text(face, x, y, chunk.text, color);

                x += width;
            }

            y += max_line_height;
        }
    }

    void text_editor::delete_selection() {
        _selection.normalize();

        auto row = _selection.start().row;
        auto last_row = _selection.end().row;

        if (row == last_row) {
            auto line_end = _document.find_line_end(row);
            if (_selection.start().column == 0 && _selection.end().column == line_end) {
                _document.delete_line(row);
            } else {
                for (uint16_t col = _selection.start().column; col < _selection.end().column; col++)
                    _document.put(row, col, core::element_t {0, _document.default_attr()});
            }
        } else {
            _document.delete_line(row);
            for (auto i = row; i < last_row; i++)
                _document.delete_line(row);
            for (uint16_t col = 0; col < _selection.end().column; col++)
                _document.put(row, col, core::element_t {0, _document.default_attr()});
        }

        _caret.row(static_cast<uint8_t>(_selection.start().row));
        _caret.column(static_cast<uint8_t>(_selection.start().column));
        update_virtual_position();
        end_selection();
    }

    void text_editor::calculate_page_metrics() {
        auto rect = bounds();
        if (rect.empty())
            return;
        _metrics.page_width = static_cast<uint8_t>((rect.width() - _metrics.line_number_width) / font_face()->width);
        _metrics.page_height = static_cast<uint8_t>(rect.height() / font_face()->line_height);
    }

    void text_editor::insert_text(const char* text) {
        const char* c = text;
        while (true) {
            if (*c == '\0')
                break;

            if (*c == '\n') {
                if (_caret.mode() == core::caret::mode::insert)
                    _document.split_line(_vrow, _vcol);
                caret_newline();
            } else {
                if (_caret.mode() == core::caret::mode::insert)
                    _document.shift_line_right(_vrow, _vcol);
                _document.put(
                        _vrow,
                        _vcol,
                        core::element_t {static_cast<uint8_t>(*c), _document.default_attr()});
                caret_right();
            }

            c++;
        }
    }

    void text_editor::on_resize(const core::rect& context_bounds) {
        core::view::on_resize(context_bounds);

        calculate_page_metrics();

        _caret.page_size(_metrics.page_height, _metrics.page_width);
        _document.page_size(_metrics.page_height, _metrics.page_width);

        update_virtual_position();
    }

    void text_editor::initialize(uint32_t rows, uint16_t columns) {
        _metrics.line_number_width = font_face()->measure_chars(5) + 2;

        _vcol = 0;
        _vrow = 0;

        _caret.palette(palette());
        _caret.on_caret_changed([&]() {
            raise_caret_changed();
        });
        _caret.font_family(font_family());
        _caret.padding().left(_metrics.line_number_width);
        _caret.initialize(0, 0);

        _document.on_document_changed([&]() {
            raise_caret_changed();
        });
        _document.default_attr(core::attr_t {fg_color(), core::font::styles::normal});
        _document.document_size(rows, columns);
        _document.clear();

        add_child(&_caret);
        margin({_metrics.left_padding, _metrics.right_padding, 5, 5});

        listen_for_on_container_change();
        bind_events();
    }

    void text_editor::get_selected_text(std::stringstream& stream) {
        _selection.normalize();
        auto row = _selection.start().row;
        auto last_row = _selection.end().row;

        if (row == last_row) {
            _document.write_line(stream, row, _selection.start().column, _selection.end().column);
            stream << "\n";
        } else {
            _document.write_line(stream, row, _selection.start().column, _document.columns());
            stream << "\n";
            ++row;
            if (row == last_row) {
                _document.write_line(stream, last_row, 0, _selection.end().column);
                stream << "\n";
            } else {
                for (; row < last_row; ++row) {
                    _document.write_line(stream, row, 0, _document.columns());
                    stream << "\n";
                }
                _document.write_line(stream, last_row, 0, _selection.end().column);
                stream << "\n";
            }
        }
    }

    bool text_editor::load(core::result& result, const fs::path& path) {
        return _document.load(result, path);
    }

    bool text_editor::save(core::result& result, const fs::path& path) {
        return _document.save(result, path);
    }

    bool text_editor::input_event_filter(const core::event_data_t& data) {
        return focused();
    }

    void text_editor::on_caret_changed(const caret_changed_callable& callable) {
        _caret_changed_callback = callable;
    }

    void text_editor::for_each_selection_char(const text_editor::char_action_callable& action) {
        _selection.normalize();
        auto row = _selection.start().row;
        auto col = _selection.start().column;
        auto line_end = _document.find_line_end(row);
        while (true) {
            action(row, col);
            col++;
            if (col >= line_end
            || (_selection.end().column > 0 && col > _selection.end().column && row == _selection.end().row)) {
                col = 0;
                row++;
                if (row > _selection.end().row)
                    break;
                line_end = _document.find_line_end(row);
            }
        }
    }

}