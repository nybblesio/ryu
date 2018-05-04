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
#include <core/project.h>
#include <common/bytes.h>
#include <core/document.h>
#include "memory_editor.h"

namespace ryu::core {

    memory_editor::memory_editor(
            const std::string& name,
            core::view_host* host) : core::view(core::view::types::container, name, host),
                                     _caret("editor-caret", host) {
    }

    memory_editor::~memory_editor() {
    }

    void memory_editor::clear() {
        _caret.row(0);
        _caret.column(0);
    }

    void memory_editor::caret_left() {
        if (_caret.left(1)) {
            _caret.column(static_cast<uint8_t>((_metrics.page_width * 3) + _metrics.page_width + 1));
            _caret.up(1);
            _nybble = 1;
        } else {
            auto column = _caret.column() + 1;
            if (column < (_metrics.page_width * 3) + 1 && column % 3 == 0) {
                _caret.left(1);
                _nybble = 1;
            } else {
                _nybble = 0;
            }
        }
    }

    void memory_editor::caret_right(uint8_t overflow_column) {
        if (_caret.right(1)) {
            _caret.column(overflow_column);
            _caret.down(1);
            _nybble = 0;
        } else {
            auto column = _caret.column() + 1;
            if (column < (_metrics.page_width * 3) + 1 && column % 3 == 0) {
                _caret.right(1);
                _nybble = 0;
            } else {
                _nybble = 1;
            }
        }
    }

    void memory_editor::bind_events() {
        action_provider().register_handler(
                core::input_action::find_by_name("memory_editor_tab"),
                [this](const core::event_data_t& data) {
                    auto ascii_column_start = _metrics.page_width * 3;
                    if (_caret.column() < ascii_column_start)
                        _caret.column(static_cast<uint8_t>(ascii_column_start));
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("memory_editor_shift_tab"),
                [this](const core::event_data_t& data) {
                    auto ascii_column_start = _metrics.page_width * 3;
                    if (_caret.column() >= ascii_column_start)
                        _caret.column(0);
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("memory_editor_home"),
                [this](const core::event_data_t& data) {
                    _caret.column(0);
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("memory_editor_end"),
                [this](const core::event_data_t& data) {
                    _caret.column(static_cast<uint8_t>((_metrics.page_width * 3) + _metrics.page_width + 1));
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("memory_editor_caret_left"),
                [this](const core::event_data_t& data) {
                    caret_left();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("memory_editor_caret_right"),
                [this](const core::event_data_t& data) {
                    caret_right();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("memory_editor_caret_up"),
                [this](const core::event_data_t& data) {
                    if (_caret.up(1)) {
                        if (_address > _metrics.page_width)
                            _address -= _metrics.page_width;
                    }
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("memory_editor_caret_down"),
                [this](const core::event_data_t& data) {
                    if (_caret.down(1)) {
                        auto machine = core::project::instance()->machine();
                        if (_address < machine->mapper()->address_space() - _metrics.page_width)
                            _address += _metrics.page_width;
                    }
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("memory_editor_page_up"),
                [this](const core::event_data_t& data) {
                    uint32_t page_size = _metrics.page_height * _metrics.page_width;
                    if (page_size > _address)
                        _address = 0;
                    else
                        _address -= page_size;
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("memory_editor_page_down"),
                [this](const core::event_data_t& data) {
                    auto machine = core::project::instance()->machine();
                    uint32_t page_size = _metrics.page_height * _metrics.page_width;
                    uint32_t remaining_memory = machine->mapper()->address_space() - _address;
                    if (page_size > remaining_memory)
                        _address += remaining_memory;
                    else
                        _address += page_size;
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("memory_editor_input"),
                [this](const core::event_data_t& data) {
                    auto machine = core::project::instance()->machine();

                    if (data.c == core::ascii_escape)
                        return false;

                    auto ascii_column_start = _metrics.page_width * 3;

                    if (_caret.column() < ascii_column_start) {
                        if (!isxdigit(data.c))
                            return false;

                        auto offset = (_caret.row() * _metrics.page_width) +
                                      _caret.column() / 3;

                        auto nybble = static_cast<uint8_t>(data.c > '9' ?
                            (data.c & ~0x20) - 'A' + 10 :
                            data.c - '0');

                        auto value = machine
                                ->mapper()
                                ->read_byte(static_cast<uint32_t>(_address + offset));
                        if (_nybble == 0) {
                            value = set_upper_nybble(value, nybble);
                        } else {
                            value = set_lower_nybble(value, nybble);
                        }

                        machine->mapper()->write_byte(_address + offset, value);
                        caret_right();
                    } else {
                        if (!isprint(data.c))
                            return false;

                        auto offset = (_caret.row() * _metrics.page_width) +
                                      (_caret.column() - ascii_column_start);
                        machine->mapper()->write_byte(
                                _address + offset,
                                static_cast<uint8_t>(data.c));
                        caret_right(static_cast<uint8_t>(ascii_column_start));
                    }

                    return true;
                });
    }

    void memory_editor::on_initialize() {
        tab_stop(true);

        define_actions();
        bind_events();

        _metrics.address_width = font_face()->measure_chars(9) + 8;

        _caret.initialize();
        _caret.palette(palette());
        _caret.on_caret_changed([&]() {
            raise_caret_changed();
        });
        _caret.font_family(font_family());
        _caret.padding().left(_metrics.address_width);
        _caret.position(0, 0);

        add_child(&_caret);
        margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
    }

    void memory_editor::define_actions() {
        auto caret_left_action = core::input_action::create(
                "memory_editor_caret_left",
                "IDE::Memory Editor",
                "Move the caret left.");
        if (!caret_left_action->has_bindings()) {
            caret_left_action->bind_keys({core::key_left});
            caret_left_action->bind_keys({core::key_backspace});
        }

        auto caret_select_left_action = core::input_action::create(
                "memory_editor_caret_select_left",
                "IDE::Memory Editor",
                "Move the caret left in selection mode.");
        if (!caret_select_left_action->has_bindings()) {
            caret_select_left_action->bind_keys({core::mod_shift, core::key_left});
        }

        auto caret_right_action = core::input_action::create(
                "memory_editor_caret_right",
                "IDE::Memory Editor",
                "Move the caret right.");
        if (!caret_right_action->has_bindings()) {
            caret_right_action->bind_keys({core::key_right});
        }

        auto caret_select_right_action = core::input_action::create(
                "memory_editor_caret_select_right",
                "IDE::Memory Editor",
                "Move the caret right in selection mode.");
        if (!caret_select_right_action->has_bindings()) {
            caret_select_right_action->bind_keys({core::mod_shift, core::key_right});
        }

        auto caret_down_action = core::input_action::create(
                "memory_editor_caret_down",
                "IDE::Memory Editor",
                "Move the caret down.");
        if (!caret_down_action->has_bindings()) {
            caret_down_action->bind_keys({core::key_down});
        }

        auto caret_select_down_action = core::input_action::create(
                "memory_editor_caret_select_down",
                "IDE::Memory Editor",
                "Move the caret down in selection mode.");
        if (!caret_select_down_action->has_bindings()) {
            caret_select_down_action->bind_keys({core::mod_shift, core::key_down});
        }

        auto caret_up_action = core::input_action::create(
                "memory_editor_caret_up",
                "IDE::Memory Editor",
                "Move the caret up a line.");
        if (!caret_up_action->has_bindings()) {
            caret_up_action->bind_keys({core::key_up});
        }

        auto caret_select_up_action = core::input_action::create(
                "memory_editor_caret_select_up",
                "IDE::Memory Editor",
                "Move the caret up in selection mode.");
        if (!caret_select_up_action->has_bindings()) {
            caret_select_up_action->bind_keys({core::mod_shift, core::key_up});
        }

        auto page_up_action = core::input_action::create(
                "memory_editor_page_up",
                "IDE::Memory Editor",
                "Move up one page.");
        if (!page_up_action->has_bindings()) {
            page_up_action->bind_keys({core::key_page_up});
        }

        auto page_down_action = core::input_action::create(
                "memory_editor_page_down",
                "IDE::Memory Editor",
                "Move down one page.");
        if (!page_down_action->has_bindings()) {
            page_down_action->bind_keys({core::key_page_down});
        }

        auto select_home_action = core::input_action::create(
                "memory_editor_select_home",
                "IDE::Memory Editor",
                "Move the caret to the home position in selection mode.");
        if (!select_home_action->has_bindings()) {
            select_home_action->bind_keys({core::mod_shift, core::key_home});
        }

        auto tab_action = core::input_action::create(
                "memory_editor_tab",
                "IDE::Memory Editor",
                "Move caret to the ASCII area.");
        if (!tab_action->has_bindings()) {
            tab_action->bind_keys({core::key_tab});
        }

        auto shift_tab_action = core::input_action::create(
                "memory_editor_shift_tab",
                "IDE::Memory Editor",
                "Move caret to the byte area.");
        if (!shift_tab_action->has_bindings()) {
            shift_tab_action->bind_keys({core::mod_shift, core::key_tab});
        }

        auto home_action = core::input_action::create(
                "memory_editor_home",
                "IDE::Memory Editor",
                "Move the caret to the home position.");
        if (!home_action->has_bindings()) {
            home_action->bind_keys({core::key_home});
        }

        auto select_end_action = core::input_action::create(
                "memory_editor_select_end",
                "IDE::Memory Editor",
                "Move the caret to the end position in selection mode.");
        if (!select_end_action->has_bindings()) {
            select_end_action->bind_keys({core::mod_shift, core::key_end});
        }

        auto end_action = core::input_action::create(
                "memory_editor_end",
                "IDE::Memory Editor",
                "Move the caret to the end position.");
        if (!end_action->has_bindings()) {
            end_action->bind_keys({core::key_end});
        }

        auto text_input_action = core::input_action::create_no_map(
                "memory_editor_input",
                "IDE::Memory Editor",
                "Any hexadecimal input (non-mappable).");
        if (!text_input_action->has_bindings()) {
            text_input_action->bind_text_input();
        }
    }

    void memory_editor::on_focus_changed() {
        _caret.enabled(focused());
    }

    void memory_editor::raise_caret_changed() {
        if (_caret_changed_callback != nullptr)
            _caret_changed_callback(_caret);
    }

    void memory_editor::calculate_page_metrics() {
        auto rect = client_bounds();
        if (rect.empty())
            return;
        _metrics.page_width = 16;
        _metrics.page_height = static_cast<uint8_t>(rect.height() / font_face()->line_height);
    }

    void memory_editor::address(uint32_t address) {
        _address = address;
    }

    void memory_editor::on_draw(core::renderer& surface) {
        auto machine = core::project::instance()->machine();

        auto bounds = client_bounds();
        auto y = bounds.top();
        auto pal = *palette();
        auto& text_color = pal[fg_color()];
        auto& address_color = pal[_address_color];
        auto max_line_height = font_face()->line_height;
        auto x = bounds.left() + _caret.padding().left();

        auto offset = 0;
        auto bytes_text_width = 0;
        std::stringstream byte_stream;
        std::stringstream ascii_stream;
        for (size_t row = 0; row < _metrics.page_height; row++) {
            if (_address + offset > machine->mapper()->address_space())
                break;

            surface.draw_text(
                    font_face(),
                    bounds.left(),
                    y,
                    fmt::format("{0:08x}", _address + offset),
                    address_color);

            byte_stream.str("");
            ascii_stream.str("");
            for (size_t col = 0; col < _metrics.page_width; col++) {
                auto value = machine
                        ->mapper()
                        ->read_byte(static_cast<uint32_t>(_address + offset + col));
                byte_stream << fmt::format("{:02x} ", value);
                auto c = isprint(value) ? static_cast<char>(value) : '.';
                ascii_stream << c;
            }

            auto byte_stream_str = byte_stream.str();
            if (bytes_text_width == 0) {
                bytes_text_width = font_face()->measure_text(byte_stream_str);
            }

            surface.draw_text(
                font_face(),
                x,
                y,
                byte_stream_str,
                text_color);

            surface.draw_text(
                font_face(),
                x + bytes_text_width,
                y,
                ascii_stream.str(),
                text_color);

            offset += _metrics.page_width;
            y += max_line_height;
        }
    }

    void memory_editor::caret_color(palette_index value) {
        _caret.fg_color(value);
    }

    void memory_editor::address_color(palette_index value) {
        _address_color = value;
    }

    void memory_editor::selection_color(palette_index value) {
        _selection_color = value;
    }

    void memory_editor::on_resize(const core::rect& context_bounds) {
        core::view::on_resize(context_bounds);
        calculate_page_metrics();
        _caret.page_size(
                _metrics.page_height,
                static_cast<uint8_t>((_metrics.page_width * 3) + 16));
    }

    void memory_editor::on_caret_changed(const caret_changed_callable& callable) {
        _caret_changed_callback = callable;
    }

}