//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <core/timer.h>
#include <core/engine.h>
#include <core/context.h>
#include "controller.h"

namespace ryu::emulator {

    controller::controller(
            core::context* context,
            int id,
            const std::string& name) : core::state(context, id, name) {
    }

    controller::~controller() {
    }

    void controller::on_update(uint32_t dt) {
    }

    void controller::on_init(SDL_Renderer* renderer) {
        _font = context()->engine()->find_font("topaz-8");
    }

    void controller::on_draw(SDL_Renderer* renderer) {
    }

    bool controller::on_process_event(const SDL_Event* e) {
        return false;
    }

}