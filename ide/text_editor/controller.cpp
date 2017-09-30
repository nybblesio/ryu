//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include <ide/context.h>
#include <core/engine.h>
#include <common/string_support.h>
#include "controller.h"

namespace ryu::ide::text_editor {

    controller::controller(
            core::context* context,
            const std::string& name) : core::state(context, name),
                                      _view(context, nullptr, "text-editor") {
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
        _view.initialize(rows, columns);
        _view.on_transition([&](auto* state, const std::string& name, const core::parameter_dict& params) {
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
        _initialized = true;
    }

    void controller::on_update(uint32_t dt) {
    }

    bool controller::on_process_event(const SDL_Event* e) {
        return _view.process_event(e);
    }

}