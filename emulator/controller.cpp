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

    controller::controller(const std::string& name) : core::state(name) {
    }

    controller::~controller() {
    }

    void controller::on_initialize() {
    }

    bool controller::on_load(core::result& result) {
        return true;
    }

    void controller::on_draw(core::renderer& surface) {
    }

    void controller::on_resize(const core::rect& bounds) {
    }

    void controller::on_update(uint32_t dt, core::pending_event_list& events) {
    }

}