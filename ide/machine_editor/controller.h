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

#include <core/state.h>
#include <hardware/machine.h>
#include "editor_view.h"

namespace ryu::ide::machine_editor {

    class controller : public ryu::core::state {
    public:
        explicit controller(const std::string& name);

        hardware::machine* machine();

        void machine(hardware::machine* value);

    protected:
        void on_deactivate() override;

        void on_initialize() override;

        void on_update(uint32_t dt) override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const core::rect& bounds) override;

        bool on_process_event(const SDL_Event* e) override;

        void on_activate(const core::parameter_dict& params) override;

    private:
        editor_view _view;
        hardware::machine* _machine;
    };

};

