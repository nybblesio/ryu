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
            const std::string& name,
            core::view_host* host) : core::view(types::container, name, host) {
    }

    panel::~panel() {
    }

    void panel::on_initialize() {
        should_clip(true);
    }

    void panel::on_draw(core::renderer& surface) {
        auto bounds = inner_bounds();
        surface.push_blend_mode(SDL_BLENDMODE_BLEND);
        auto current_palette = palette();
        if (current_palette != nullptr) {
            auto& bg = current_palette->get(bg_color());
            surface.set_color(bg);
            surface.fill_rect(bounds);
            surface.pop_blend_mode();
        }
    }

}