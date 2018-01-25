//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <core/engine.h>
#include <ide/ide_context.h>
#include "controller.h"

namespace ryu::ide::hex_editor {

    controller::controller(const std::string& name) : core::state(name) {
    }

    void controller::on_initialize() {
    }

    void controller::on_update(uint32_t dt) {
    }

    void controller::on_draw(core::renderer& surface) {
    }

    bool controller::on_process_event(const SDL_Event* e) {
        if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_ESCAPE: {
                    end_state();
                    return true;
                }
            }
        }
        return false;
    }

}