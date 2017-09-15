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

    caret::caret(
            core::context* context,
            core::view* parent,
            int id,
            const std::string& name) : core::view(context,
                                                  parent,
                                                  core::view::types::custom,
                                                  id,
                                                  name) {
    }

    caret::~caret() {
        delete _timer;
        _timer = nullptr;
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

    bool caret::clamp_column() {
        if (_column < 0) {
            _column = 0;
            return true;
        }
        if (_column > _page_width) {
            _column = _page_width;
            return true;
        }
        return false;
    }

    bool caret::column(int column) {
        _column = column;
        return clamp_column();
    }

    int caret::column() const {
        return _column;
    }

    bool caret::left(int columns) {
        _column -= columns;
        return clamp_column();
    }

    bool caret::right(int columns) {
        _column += columns;
        return clamp_column();
    }

    bool caret::row(int row) {
        _row = row;
        return clamp_row();
    }

    int caret::row() const {
        return _row;
    }

    bool caret::down(int rows) {
        _row += rows;
        return clamp_row();
    }

    bool caret::up(int rows) {
        _row -= rows;
        return clamp_row();
    }

    void caret::select() {
        _mode = mode::types::select;
    }

    void caret::insert() {
        _mode = mode::types::insert;
    }

    void caret::overwrite() {
        _mode = mode::types::overwrite;
    }

    void caret::column_select() {
        _mode = mode::types::column_select;
    }

    caret::mode::types caret::mode() const {
        return _mode;
    }

    void caret::on_draw() {
        if (!enabled())
            return;

        push_blend_mode(SDL_BLENDMODE_BLEND);

        auto caret_color = (*context()->palette())[fg_color()];
        caret_color.alpha(0x7f);
        set_color(caret_color);

        auto parent_bounds = parent()->client_rect();
        auto& bounds = rect();
        auto& pad = padding();
        bounds.pos(
                (parent_bounds.left() + (_column * bounds.width())) + pad.left(),
                (parent_bounds.top() + (_row * bounds.height())) + pad.top());
        bounds.size(font()->width, font()->line_height);
        fill_rect(bounds);

        pop_blend_mode();
    }

    void caret::initialize(int row, int column, int page_width, int page_height) {
        _row = row;
        _column = column;
        _page_width = page_width;
        _page_height = page_height;

        _timer = new timer(ids::blink_timer, 500);
        _timer->bind([&](timer* t) {
            visible(!visible());
            t->reset();
        });
        context()->add_timer(_timer);
    }

}