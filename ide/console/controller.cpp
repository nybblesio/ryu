//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <sstream>
#include <iostream>
#include <fmt/format.h>
#include <core/engine.h>
#include <ide/context.h>
#include "controller.h"

namespace ryu::ide::console {

    controller::controller(
            core::context* context,
            int id,
            const std::string& name) : core::state(context, id, name),
                                       _view(context, nullptr, ids::console, "console") {
    }

    controller::~controller() {
    }

    void controller::on_draw() {
        _view.draw();
    }

    void controller::on_initialize() {
        _view.font(context()->engine()->find_font("hack-normal"));
        _view.initialize();
        _view.on_transition([&](const std::string& name) {
            return transition_to(name);
        });
        _view.on_execute_command([&](core::result& result, const std::string& input) {
            _command_factory.execute(result, input);
        });

        _view.write_message("{bold}Ryu{}: {italic}The Arcade Construction Kit{}");
        _view.write_message("Copyright (C) 2017 Jeff Panici");
        _view.write_message("See details in {bold}LICENSE{} file");
        _view.caret_down();
        _view.write_message("Ready.");
        _view.focus(ids::console);
    }

    void controller::on_update(uint32_t dt) {
    }

    bool controller::on_process_event(const SDL_Event* e) {
        return _view.process_event(e);
    }

}