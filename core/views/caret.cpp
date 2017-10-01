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
#include "caret.h"

namespace ryu::core {

    caret::caret(const std::string& name) : core::view(core::view::types::control, name),
                                            _timer(500) {
    }

    void caret::select() {
        _mode = mode::types::select;
    }

    void caret::insert() {
        _mode = mode::types::insert;
    }

    int caret::row() const {
        return _row;
    }

    void caret::overwrite() {
        _mode = mode::types::overwrite;
    }

    bool caret::clamp_row() {
        if (_row < 0) {
            _row = 0;
            return true;
        }
        if (_row > _page_height - 1) {
            _row = _page_height - 1;
            return true;
        }
        return false;
    }

    bool caret::row(int row) {
        _row = row;
        return clamp_row();
    }

    bool caret::up(int rows) {
        _row -= rows;
        return clamp_row();
    }

    int caret::column() const {
        return _column;
    }

    bool caret::clamp_column() {
        if (_column < 0) {
            _column = 0;
            return true;
        }
        if (_column > _page_width - 1) {
            _column = _page_width - 1;
            return true;
        }
        return false;
    }

    bool caret::down(int rows) {
        _row += rows;
        return clamp_row();
    }

    void caret::column_select() {
        _mode = mode::types::column_select;
    }

    bool caret::left(int columns) {
        _column -= columns;
        return clamp_column();
    }

    bool caret::right(int columns) {
        _column += columns;
        return clamp_column();
    }

    bool caret::column(int column) {
        _column = column;
        return clamp_column();
    }

    caret::mode::types caret::mode() const {
        return _mode;
    }

    void caret::initialize(int row, int column) {
        _row = row;
        _column = column;

        _timer.bind([&](timer* t) {
            visible(!visible());
            t->reset();
        });
        // TODO:
        //context()->add_timer(&_timer);
    }

    void caret::on_draw(SDL_Renderer* renderer) {
        if (!enabled())
            return;

        push_blend_mode(renderer, SDL_BLENDMODE_BLEND);

        auto pal = *palette();
        auto caret_color = pal[fg_color()];
        caret_color.alpha(0x7f);
        set_color(renderer, caret_color);

        auto parent_bounds = parent()->client_bounds();
        auto& rect = bounds();
        auto& pad = padding();
        rect.pos(
                (parent_bounds.left() + (_column * rect.width())) + pad.left(),
                (parent_bounds.top() + (_row * rect.height())) + pad.top());
        rect.size(font_face()->width, font_face()->line_height);
        fill_rect(renderer, rect);

        pop_blend_mode(renderer);
    }

    void caret::page_size(int page_height, int page_width) {
        _page_width = page_width;
        _page_height = page_height;
    }

}