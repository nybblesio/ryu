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
#include "memory_editor.h"

namespace ryu::core {

    memory_editor::memory_editor(
            const std::string& name,
            core::view_container* container) : core::view(core::view::types::container, name, container),
                                               _caret("editor-caret", container) {
    }

    void memory_editor::clear() {

        first_page();
        _caret.row(0);
        _caret.column(0);
        update_virtual_position();
    }

    void memory_editor::page_up() {

        update_virtual_position();
    }

    void memory_editor::page_down() {

        update_virtual_position();
    }

    void memory_editor::scroll_up() {

        update_virtual_position();
    }

    void memory_editor::caret_end() {
        auto end_column = 2 * 16;
        caret_home();
        for (auto i = 0; i < end_column; i++)
            caret_right();
        update_virtual_position();
    }

    void memory_editor::last_page() {
        update_virtual_position();
    }

    void memory_editor::first_page() {
        update_virtual_position();
    }

    void memory_editor::caret_home() {
        _caret.column(0);
        update_virtual_position();
    }

    void memory_editor::scroll_down() {
        update_virtual_position();
    }

    void memory_editor::scroll_left() {
        update_virtual_position();
    }

    bool memory_editor::scroll_right() {
        bool clamped = false;
        update_virtual_position();
        return clamped;
    }

    void memory_editor::end_selection() {
        if (_caret.mode() != core::caret::mode::select)
            return;

        _caret.insert();
        _selection.end(_vrow, _vcol);
    }

    void memory_editor::on_focus_changed() {
        _caret.enabled(focused());
    }

    void memory_editor::raise_caret_changed() {
        if (_caret_changed_callback != nullptr)
            _caret_changed_callback(_caret);
    }

    void memory_editor::caret_up(uint8_t rows) {
        if (_caret.up(rows))
            scroll_up();
        update_virtual_position();
    }

    void memory_editor::goto_address(uint32_t address) {

        update_virtual_position();
    }

    void memory_editor::caret_down(uint8_t rows) {
        if (_caret.down(rows))
            scroll_down();
        update_virtual_position();
    }

    void memory_editor::update_virtual_position() {
        _vrow = _caret.row();
        _vcol = _caret.column();

        if (_caret.mode() != core::caret::mode::select)
            _selection.clear();
    }

    void memory_editor::caret_color(uint8_t value) {
        _caret.fg_color(value);
    }

    void memory_editor::caret_left(uint8_t columns) {
        if (_caret.left(columns))
            scroll_left();
        update_virtual_position();
    }

    bool memory_editor::caret_right(uint8_t columns) {
        auto clamped = false;
        if (_caret.right(columns)) {
            clamped = scroll_right();
            if (clamped) {
                caret_down();
                caret_home();
            }
        }
        update_virtual_position();
        return clamped;
    }

    void memory_editor::selection_color(uint8_t value) {
        _selection_color = value;
    }

    void memory_editor::find(const std::string& needle) {
    }

    void memory_editor::address_color(uint8_t value) {
        _line_number_color = value;
    }

    void memory_editor::update_selection(uint16_t line_end) {
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

    void memory_editor::on_draw(core::renderer& surface) {
//        auto bounds = client_bounds();
//        auto pal = *palette();

//        auto& info_text_color = pal[_line_number_color];

        // XXX: this may not be correct because the font metrics may change
//        auto face = font_face();
//        auto y = bounds.top();

//        auto row_start = _document.row();
//        auto row_stop = row_start + _metrics.page_height;
//
//        for (auto row = row_start; row < row_stop; row++) {
//            surface.draw_text(font_face(), bounds.left(), y, fmt::format("{0:04}", row + 1), info_text_color);
//
//            uint16_t col_start = static_cast<uint16_t>(_document.column());
//            uint16_t col_end = col_start + _metrics.page_width;
//
//            auto x = bounds.left() + _caret.padding().left();
//            auto chunks = _document.get_line_chunks(
//                    static_cast<uint32_t>(row),
//                    col_start,
//                    col_end);
//            for (const auto& chunk : chunks) {
//                auto width = static_cast<int32_t>(face->width * chunk.text.length());
//                auto color = pal[chunk.attr.color];
//
//                if ((chunk.attr.flags & core::font::flags::reverse) != 0) {
//                    surface.push_blend_mode(SDL_BLENDMODE_BLEND);
//                    auto selection_color = pal[_selection_color];
//                    selection_color.alpha(0x7f);
//                    surface.set_color(selection_color);
//                    surface.fill_rect(core::rect{x, y, width, face->line_height});
//                    surface.pop_blend_mode();
//                }
//
//                font_style(chunk.attr.style);
//                surface.draw_text(font_face(), x, y, chunk.text, color);
//                x += width;
//            }
//
//            y += face->line_height;
//        }
    }

    void memory_editor::delete_selection() {
        _selection.normalize();

//        auto row = _selection.start().row;
//        auto last_row = _selection.end().row;

//        if (row == last_row) {
//            auto line_end = _document.find_line_end(row);
//            if (_selection.start().column == 0 && _selection.end().column == line_end) {
//                _document.delete_line(row);
//            } else {
//                for (uint16_t col = _selection.start().column; col < _selection.end().column; col++)
//                    _document.put(row, col, core::element_t {0, _document.default_attr()});
//            }
//        } else {
//            _document.delete_line(row);
//            for (auto i = row; i < last_row; i++)
//                _document.delete_line(row);
//            for (uint16_t col = 0; col < _selection.end().column; col++)
//                _document.put(row, col, core::element_t {0, _document.default_attr()});
//        }

        _caret.row(static_cast<uint8_t>(_selection.start().row));
        _caret.column(static_cast<uint8_t>(_selection.start().column));

        update_virtual_position();
        end_selection();
    }

    void memory_editor::calculate_page_metrics() {
        auto rect = bounds();
        if (rect.empty())
            return;
        _metrics.page_width = static_cast<uint8_t>((rect.width() - _metrics.line_number_width) / font_face()->width);
        _metrics.page_height = static_cast<uint8_t>(rect.height() / font_face()->line_height);
    }

    void memory_editor::on_resize(const core::rect& context_bounds) {
        core::view::on_resize(context_bounds);

        calculate_page_metrics();

        _caret.page_size(_metrics.page_height, _metrics.page_width);

        update_virtual_position();
    }

    void memory_editor::initialize(uint32_t rows, uint16_t columns) {
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

        add_child(&_caret);
        margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
    }

    void memory_editor::on_caret_changed(const caret_changed_callable& callable) {
        _caret_changed_callback = callable;
    }

}