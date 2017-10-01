//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <core/state.h>
#include <core/project.h>
#include <hardware/machine.h>
#include <common/SDL_FontCache.h>
#include "editor_view.h"

namespace ryu::ide::text_editor {

    class controller : public ryu::core::state {
    public:
        const int rows = 4096;
        const int columns = 128;

        explicit controller(const std::string& name);

        ~controller() override;

        core::project* project();

        hardware::machine* machine();

        void project(core::project* value);

        void machine(hardware::machine* value);

    protected:
        void on_draw() override;

        void on_resize() override;

        void on_initialize() override;

        void on_update(uint32_t dt) override;

        bool on_process_event(const SDL_Event* e) override;

    private:
        editor_view _view;
        environment _command_factory;
        core::project* _project = nullptr;
        hardware::machine* _machine = nullptr;
    };

};
