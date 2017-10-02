//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include <core/engine.h>
#include <ide/ide_context.h>
#include "controller.h"

namespace ryu::ide::text_editor {

    controller::controller(const std::string& name) : core::state(name),
                                                      _view("text-editor") {
    }

    controller::~controller() {
    }

    void controller::on_initialize() {
        _view.palette(context()->palette());
        _view.font_family(context()->engine()->find_font_family("hack"));
        _view.initialize(rows, columns);
        _view.on_transition([&](const std::string& name, const core::parameter_dict& params) {
            return transition_to(name, params);
        });
        _view.on_execute_command([&](core::result& result, const std::string& input) {
            _command_factory.execute(result, input);
            if (result.has_code("C001"))
                context()->engine()->quit();
            else if (result.has_code("C004")) {
                _view.clear();
            } else if (result.has_code("C020")) {
                _view.goto_line(std::atoi(result.messages()[0].message().c_str()));
            } else if (result.has_code("C021")) {
                _view.load(result.messages()[0].message());
            } else if (result.has_code("C022")) {
                auto path = result.messages()[0].message();
                _view.save(path == "(default)" ? "" : path);
            }
        });
        _view.focus(_view.id());
    }

    core::project* controller::project() {
        return _project;
    }

    void controller::on_update(uint32_t dt) {
    }

    hardware::machine* controller::machine() {
        return _machine;
    }

    void controller::project(core::project* value) {
        _project = value;
    }

    void controller::machine(hardware::machine* value) {
        _machine = value;
    }

    void controller::on_draw(core::renderer& surface) {
        _view.draw(surface);
    }

    void controller::on_resize(const core::rect& bounds) {
        _view.resize(bounds);
    }

    bool controller::on_process_event(const SDL_Event* e) {
        auto ctrl_pressed = (SDL_GetModState() & KMOD_CTRL) != 0;

        if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_ESCAPE: {
                    if (!ctrl_pressed) {
                        end_state();
                        return true;
                    }
                    break;
                }
            }
        }
        return _view.process_event(e);
    }

}