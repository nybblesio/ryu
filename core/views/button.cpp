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
            core::view* parent,
            const std::string& name) : core::view(context,
                                                  parent,
                                                  core::view::types::custom,
                                                  name) {
    }

    button::~button() {
    }

    std::string button::value() const {
        return _value;
    }

    alignment::types button::alignment() const {
        return _alignment;
    }

    void button::value(const std::string& value) {
        _value = value;
    }

    void button::alignment(alignment::types value) {
        _alignment = value;
    }

    void button::on_draw() {
        push_blend_mode(SDL_BLENDMODE_BLEND);

        auto bounds = client_rect();

        auto fg = (*context()->palette())[fg_color()];
        auto bg = (*context()->palette())[bg_color()];

        if (!enabled()) {
            fg = fg.fade(2);
            bg = bg.fade(2);
        }

        set_color(bg);
        fill_rect(bounds);
        set_color(fg);
        draw_rect(bounds);

        set_font_color(fg);
        draw_text_aligned(_value, bounds, _alignment);

        pop_blend_mode();
    }

    bool button::on_process_event(const SDL_Event* e) {
        return false;
    }

    void button::on_clicked(const button::on_clicked_callable& callable) {
        _on_clicked = callable;
    }

}