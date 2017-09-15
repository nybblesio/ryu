//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//


#pragma once

#include <SDL_events.h>
#include <core/state.h>
#include <core/views/caret.h>
#include <ide/command_factory.h>
#include "view.h"

namespace ryu::ide::console {

    class controller : public core::state {
    public:
        enum ids {
            console = 1
        };

        controller(
                core::context* context,
                int id,
                const std::string& name);

        ~controller() override;

    protected:
        void on_draw() override;

        void on_initialize() override;

        void on_update(uint32_t dt) override;

        bool on_process_event(const SDL_Event* e) override;

    private:
        console::view _view;
        command_factory _command_factory;
    };

};
