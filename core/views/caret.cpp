//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <core/timer.h>
#include <core/timer_pool.h>
#include "caret.h"

namespace ryu::core {

    caret::caret(
            const std::string& name,
            core::view_container* container) : core::view(core::view::types::control, name, container),
                                               _timer(500) {
    }

    void caret::select() {
        _mode = mode::types::select;
        raise_caret_changed();
    }

    void caret::insert() {
        _mode = mode::types::insert;
        raise_caret_changed();
    }

    void caret::overwrite() {
        _mode = mode::types::overwrite;
        raise_caret_changed();
    }

    uint8_t caret::row() const {
        return static_cast<uint8_t>(_row);
    }

    bool caret::row(uint8_t row) {
        _row = row;
        auto clamped = clamp_row();
        raise_caret_changed();
        return clamped;
    }

    bool caret::up(uint8_t rows) {
        _row -= rows;
        auto clamped = clamp_row();
        raise_caret_changed();
        return clamped;
    }

    uint8_t caret::column() const {
        return static_cast<uint8_t>(_column);
    }

    bool caret::down(uint8_t rows) {
        _row += rows;
        auto clamped = clamp_row();
        raise_caret_changed();
        return clamped;
    }

    void caret::column_select() {
        _mode = mode::types::column_select;
        raise_caret_changed();
    }

    bool caret::left(uint8_t columns) {
        _column -= columns;
        auto clamped = clamp_column();
        raise_caret_changed();
        return clamped;
    }

    bool caret::right(uint8_t columns) {
        _column += columns;
        auto clamped = clamp_column();
        raise_caret_changed();
        return clamped;
    }

    bool caret::column(uint8_t column) {
        _column = column;
        auto clamped = clamp_column();
        raise_caret_changed();
        return clamped;
    }

    void caret::raise_caret_changed() {
        if (_caret_changed_callback != nullptr)
            _caret_changed_callback();
    }

    caret::mode::types caret::mode() const {
        return _mode;
    }

    void caret::initialize(uint8_t row, uint8_t column) {
        _row = row;
        _column = column;
        raise_caret_changed();
        _timer.bind([&](timer* t) {
            _show = !_show;
            t->reset();
        });
        timer_pool::instance()->add_timer(&_timer);
    }

    bool caret::clamp_row() {
        if (_row < 0) {
            _row = 0;
            return true;
        }
        auto clamp = _page_height > 0 ? _page_height - 1 : 0;
        if (_row > clamp) {
            _row = static_cast<int16_t>(clamp);
            return true;
        }
        return false;
    }

    bool caret::clamp_column() {
        if (_column < 0) {
            _column = 0;
            return true;
        }
        auto clamp = _page_width > 0 ? _page_width - 1 : 0;
        if (_column > clamp) {
            _column = static_cast<int16_t>(clamp);
            return true;
        }
        return false;
    }

    void caret::on_draw(core::renderer& surface) {
        if (!enabled() || !_show)
            return;

        surface.push_blend_mode(SDL_BLENDMODE_BLEND);

        // XXX: we should cache the modified color when fg_color is changed
        auto current_palette = palette();
        if (current_palette != nullptr) {
            auto caret_color = current_palette->get(fg_color());
            caret_color.alpha(0x7f);
            surface.set_color(caret_color);
        }

        auto parent_bounds = parent()->client_bounds();
        auto& rect = bounds();
        auto& pad = padding();
        rect.size(font_face()->width, font_face()->line_height);
        rect.pos(
                (parent_bounds.left() + (_column * rect.width())) + pad.left(),
                (parent_bounds.top() + (_row * rect.height())) + pad.top());
        surface.fill_rect(rect);

        surface.pop_blend_mode();
    }

    void caret::page_size(uint8_t page_height, uint8_t page_width) {
        _page_width = page_width;
        _page_height = page_height;
        clamp_row();
        clamp_column();
    }

    void caret::on_caret_changed(const caret::caret_changed_callable& callable) {
        _caret_changed_callback = callable;
    }

}