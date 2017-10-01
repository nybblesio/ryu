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
            const std::string& name) : core::state(context, name),
                                       _view(context, "console") {
    }

    controller::~controller() {
    }

    void controller::on_draw() {
        _view.draw();
    }

    void controller::on_resize() {
        if (!_initialized)
            return;
        _view.resize();
    }

    void controller::on_initialize() {
        _view.font_family(context()->engine()->find_font_family("hack"));
        _view.initialize();
        _view.on_transition([&](auto* state, const std::string& name, const core::parameter_dict& params) {
            return transition_to(name, params);
        });
        _view.on_execute_command([&](core::result& result, const std::string& input) {
            return _command_factory.execute(result, input);
        });

        _view.write_message("{rev} {bold}Ryu: {italic}The Arcade Construction Kit {}");
        _view.write_message(" Copyright (C) 2017 Jeff Panici");
        _view.write_message(" See details in {underline}{bold}LICENSE{} file");
        _view.caret_down();
        _view.write_message("Ready.");
        _view.focus(_view.id());
        _initialized = true;
    }

    void controller::on_update(uint32_t dt) {
    }

    bool controller::on_process_event(const SDL_Event* e) {
        return _view.process_event(e);
    }

}