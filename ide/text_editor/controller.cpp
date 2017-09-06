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
            int id,
            const std::string& name) : core::state(context, id, name),
                                      _view(context, nullptr, ids::text_editor, "text-editor") {
    }

    controller::~controller() {
    }

    void controller::on_update(uint32_t dt) {
    }

    void controller::on_init(SDL_Renderer* renderer) {
        _view.font(context()->engine()->find_font("hack-bold"));
        _view.initialize(rows, columns);
        _view.on_transition([&](const std::string& name) {
            return transition_to(name);
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
        _view.focus(ids::text_editor);
    }

    void controller::on_draw(SDL_Renderer* renderer) {
        _view.draw(renderer);
    }

    bool controller::on_process_event(const SDL_Event* e) {
        return _view.process_event(e);
    }

}