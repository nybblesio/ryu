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
            const std::string& name) : core::state(context, name) {
    }

    controller::~controller() {
    }

    void controller::on_draw() {
    }

    void controller::on_resize() {
    }

    void controller::on_initialize() {
    }

    void controller::on_update(uint32_t dt) {
    }

    bool controller::on_process_event(const SDL_Event* e) {
        return false;
    }

}