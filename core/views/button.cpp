//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "button.h"

namespace ryu::core {

    button::button(
            core::context* context,
            const std::string& name) : core::view(context, core::view::types::control, name) {
    }

    std::string button::value() const {
        return _value;
    }

    border::types button::border() const {
        return _border;
    }

    void button::border(border::types value) {
        _border = value;
    }

    void button::value(const std::string& value) {
        _value = value;
    }

    void button::on_draw() {
        push_blend_mode(SDL_BLENDMODE_BLEND);

        auto bounds = client_bounds();

        auto fg = (*context()->palette())[fg_color()];
        auto bg = (*context()->palette())[bg_color()];

        if (!enabled()) {
            fg = fg - 45;
            bg = bg - 45;
        } else if (!focused()) {
            fg = fg - 35;
            bg = bg - 35;
        }

        set_color(bg);
        fill_rect(bounds);

        set_font_color(fg);
        draw_text_aligned(_value, bounds, _halign, _valign);

        if (_border == border::types::solid) {
            set_color(fg);
            draw_rect(bounds);
        }

        pop_blend_mode();
    }

    bool button::on_process_event(const SDL_Event* e) {
        if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_SPACE: {
                    if (_on_clicked) {
                        _on_clicked();
                    }
                    return true;
                }
            }
        }
        return false;
    }

    alignment::vertical::types button::valign() const {
        return _valign;
    }

    alignment::horizontal::types button::halign() const {
        return _halign;
    }

    void button::valign(alignment::vertical::types value) {
        _valign = value;
    }

    void button::halign(alignment::horizontal::types value) {
        _halign = value;
    }

    void button::on_clicked(const button::on_clicked_callable& callable) {
        _on_clicked = callable;
    }

}