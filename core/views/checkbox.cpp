//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "checkbox.h"

namespace ryu::core {

    checkbox::checkbox(
            const std::string& name,
            core::view_host* host) : core::view(types::control, name, host) {
    }

    bool checkbox::value() const {
        return _value;
    }

    void checkbox::value(bool flag) {
        _value = flag;
    }

    void checkbox::on_draw(core::renderer& surface) {
        auto bounds = client_bounds();

        auto pal = *palette();
        auto& fg = pal[fg_color()];

        if (!enabled() || !focused()) {
            fg = fg - 35;
        }

        surface.set_color(fg);
        surface.draw_rect(bounds);

        if (_value) {
            surface.draw_line(bounds.left(), bounds.top(), bounds.right(), bounds.bottom());
        }
    }

//    bool checkbox::on_process_event(const SDL_Event* e) {
//        if (e->type == SDL_KEYDOWN) {
//            switch (e->key.keysym.sym) {
//                case SDLK_SPACE: {
//                    _value = !_value;
//                    return true;
//                }
//            }
//        }
//        return false;
//    }

}