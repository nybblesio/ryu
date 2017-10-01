//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "panel.h"

namespace ryu::core {

    panel::panel(const std::string& name) : core::view(types::container, name) {
    }

    void panel::on_draw(SDL_Renderer* renderer) {
        push_blend_mode(renderer, SDL_BLENDMODE_BLEND);
        auto pal = *palette();
        auto& fg = pal[fg_color()];
        auto& bg = pal[bg_color()];
        set_color(renderer, bg);
        auto client_rect = client_bounds();
        fill_rect(renderer, client_rect);
        pop_blend_mode(renderer);
        if (_border != border::types::none) {
            set_color(renderer, fg);
            draw_rect(renderer, client_rect);
        }
    }

    border::types panel::border() const {
        return _border;
    }

    void panel::border(border::types value) {
        _border = value;
    }
}