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
#include <ide/ide_context.h>
#include "controller.h"

namespace ryu::ide::console {

    controller::controller(const std::string& name) : core::state(name),
                                                      _environment(),
                                                      _view("console") {
    }

    controller::~controller() {
    }

    void controller::on_initialize() {
        _view.palette(context()->palette());
        _view.font_family(context()->engine()->find_font_family("hack"));
        _view.initialize();
        _view.on_transition([&](const std::string& name, const core::parameter_dict& params) {
            return transition_to(name, params);
        });
        _view.on_execute_command([&](core::result& result, const std::string& input) {
            auto success = _environment.execute(result, input);
            if (success && result.has_code("C001")) {
                context()->engine()->quit();
            }
            return success;
        });
        _view.focus(_view.id());
    }

    void controller::on_update(uint32_t dt) {
    }

    void controller::on_draw(core::renderer& surface) {
        _view.draw(surface);
    }

    void controller::on_resize(const core::rect& bounds) {
        _view.resize(bounds);
    }

    bool controller::on_process_event(const SDL_Event* e) {
        return _view.process_event(e);
    }

    void controller::on_activate(const core::parameter_dict& params) {
        if (_show_banner) {
            _view.write_message("{rev} {bold}Ryu: {italic}The Arcade Construction Kit {}");
            _view.write_message(" Copyright (C) 2017 Jeff Panici");
            _view.write_message(" See details in {underline}{bold}LICENSE{} file");
            _view.caret_down();
            _view.write_message("Ready.");
            _show_banner = false;
        }
    }

}