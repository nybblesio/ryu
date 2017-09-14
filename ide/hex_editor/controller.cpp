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
#include <ide/context.h>
#include <common/string_support.h>
#include "controller.h"

namespace ryu::ide::hex_editor {

    controller::controller(
            core::context* context,
            int id,
            const std::string& name) : core::state(context, id, name) {
    }

    void controller::on_update(uint32_t dt) {
    }

    void controller::on_init(SDL_Renderer* renderer) {
    }

    void controller::on_draw(SDL_Renderer* renderer) {
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