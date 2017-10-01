//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "pick_list.h"

namespace ryu::core {

    pick_list::pick_list(
            core::context* context,
            const std::string& name) : core::view(context, types::control, name) {
    }

    void pick_list::on_draw() {
        auto bounds = client_bounds();

        auto fg = (*context()->palette())[fg_color()];
        auto& bg = (*context()->palette())[bg_color()];

        if (!enabled() || !focused()) {
            fg = fg - 35;
        }

        set_color(bg);
        fill_rect(bounds);

        set_font_color(fg);
        set_color(fg);

        draw_text(bounds.left(), bounds.top(), _value, fg);
        if (_border == border::types::solid) {
            set_color(fg);
            draw_rect(bounds);
        } else {
            draw_line(bounds.left(),
                      bounds.top() + bounds.height(),
                      bounds.right() + 5,
                      bounds.top() + bounds.height());
        }
    }

    int pick_list::length() const {
        return _length;
    }

    std::string pick_list::value() {
        return _value;
    }

    void pick_list::length(int value) {
        _length = value;
    }

    option_list& pick_list::options() {
        return _options;
    }

    border::types pick_list::border() const {
        return _border;
    }

    void pick_list::border(border::types value) {
        _border = value;
    }

    void pick_list::value(const std::string& value) {
        _value = value;
    }

    bool pick_list::on_process_event(const SDL_Event* e) {
        return false;
    }

}