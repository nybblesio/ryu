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

namespace ryu::ide::machine_editor {

    class controller : public ryu::core::state {
    public:
        enum ids {
        };

        controller(
                core::context* context,
                int id,
                const std::string& name);

    protected:
        void on_draw() override;

        void on_initialize() override;

        void on_update(uint32_t dt) override;

        bool on_process_event(const SDL_Event* e) override;

    private:
    };

};

