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
#include <core/document.h>
#include <core/project.h>
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

    void memory_editor::bind_events() {
    }

    void memory_editor::define_actions() {
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
        _metrics.page_width = static_cast<uint8_t>((rect.width() - _metrics.address_width) / font_face()->width);
        _metrics.page_height = static_cast<uint8_t>(rect.height() / font_face()->line_height);
    }

    void memory_editor::address(uint32_t address) {
        _address = address;
    }

    void memory_editor::caret_color(uint8_t value) {
        _caret.fg_color(value);
    }

    void memory_editor::address_color(uint8_t value) {
        _address_color = value;
    }

    void memory_editor::selection_color(uint8_t value) {
        _selection_color = value;
    }

    void memory_editor::on_draw(core::renderer& surface) {
        auto machine = core::project::instance()->machine();

        auto bounds = client_bounds();
        auto y = bounds.top();
        auto byte_count = 16;
        auto pal = *palette();
        auto& text_color = pal[fg_color()];
        auto& address_color = pal[_address_color];
        auto max_line_height = font_face()->line_height;
        auto x = bounds.left() + _caret.padding().left();

        auto offset = 0;
        for (size_t row = 0; row < _metrics.page_height; row++) {
            surface.draw_text(
                    font_face(),
                    bounds.left(),
                    y,
                    fmt::format("{0:08x}", _address + offset),
                    address_color);

            std::stringstream byte_stream;
            std::stringstream ascii_stream;
            for (size_t col = 0; col < byte_count; col++) {
                auto value = machine
                        ->mapper()
                        ->read_byte(static_cast<uint32_t>(_address + offset + col));
                byte_stream << fmt::format("{:02x} ", value);
                auto c = isprint(value) ? static_cast<char>(value) : '.';
                ascii_stream << c;
            }

            auto bytes_length = font_face()->measure_text(byte_stream.str());
            surface.draw_text(font_face(), x, y, byte_stream.str(), text_color);
            surface.draw_text(font_face(), x + bytes_length + 8, y, ascii_stream.str(), text_color);

            offset += byte_count;
            y += max_line_height;
        }
    }

    void memory_editor::on_resize(const core::rect& context_bounds) {
        core::view::on_resize(context_bounds);
        calculate_page_metrics();
        _caret.page_size(_metrics.page_height, _metrics.page_width);
    }

    void memory_editor::on_caret_changed(const caret_changed_callable& callable) {
        _caret_changed_callback = callable;
    }

}