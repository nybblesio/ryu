//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "label.h"

namespace ryu::core {

    label::label(const std::string& name) : core::view(types::control, name) {
    }

    std::string label::value() const {
        return _value;
    }

    border::types label::border() const {
        return _border;
    }

    void label::border(border::types value) {
        _border = value;
    }

    void label::value(const std::string& value) {
        _value = value;
    }

    void label::on_draw(SDL_Renderer* renderer) {
        auto bounds = client_bounds();

        auto pal = *palette();
        auto& fg = pal[fg_color()];
        auto& bg = pal[bg_color()];

        set_color(renderer, bg);
        fill_rect(renderer, bounds);
        set_font_color(fg);
        draw_text_aligned(renderer, _value, bounds, _halign, _valign);
        if (_border != border::types::none) {
            set_color(renderer, fg);
            draw_rect(renderer, bounds);
        }
    }

    alignment::vertical::types label::valign() const {
        return _valign;
    }

    alignment::horizontal::types label::halign() const {
        return _halign;
    }

    void label::valign(alignment::vertical::types value) {
        _valign = value;
    }

    void label::halign(alignment::horizontal::types value) {
        _halign = value;
    }

}