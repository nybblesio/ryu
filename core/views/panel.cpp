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

    panel::panel(
            core::context* context,
            const std::string& name) : core::view(context, types::container, name) {
    }

    void panel::on_draw() {
        push_blend_mode(SDL_BLENDMODE_BLEND);
        auto& fg = (*context()->palette())[fg_color()];
        auto& bg = (*context()->palette())[bg_color()];
        set_color(bg);
        auto client_rect = client_bounds();
        fill_rect(client_rect);
        pop_blend_mode();
        if (_border != border::types::none) {
            set_color(fg);
            draw_rect(client_rect);
        }
    }

    border::types panel::border() const {
        return _border;
    }

    void panel::border(border::types value) {
        _border = value;
    }
}