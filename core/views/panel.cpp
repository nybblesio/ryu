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

    border::types panel::border() const {
        return _border;
    }

    void panel::border(border::types value) {
        _border = value;
    }

    void panel::on_draw(core::renderer& surface) {
        auto bounds = client_bounds();
        surface.push_clip_rect(bounds);
        surface.push_blend_mode(SDL_BLENDMODE_BLEND);
        auto pal = *palette();
        auto& fg = pal[fg_color()];
        auto& bg = pal[bg_color()];
        surface.set_color(bg);
        surface.fill_rect(bounds);
        surface.pop_blend_mode();
        if (_border != border::types::none) {
            surface.set_color(fg);
            surface.draw_rect(bounds);
        }
        surface.pop_clip_rect();
    }

}