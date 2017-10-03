//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <fmt/format.h>
#include "editor_view.h"

namespace ryu::ide::machine_editor {

    editor_view::editor_view(const std::string& name) : core::view(core::view::types::container, name) {
    }

    void editor_view::initialize() {
        margin({0, 0, 0, 0});
    }

    void editor_view::on_draw(core::renderer& surface) {
    }

    bool editor_view::on_process_event(const SDL_Event* e) {
//        auto ctrl_pressed = (SDL_GetModState() & KMOD_CTRL) != 0;
//        auto shift_pressed = (SDL_GetModState() & KMOD_SHIFT) != 0;

        if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_TAB: {
                    return true;
                }
            }
        }

        return false;
    }

}