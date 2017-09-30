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

namespace ryu::emulator {

    class controller  : public core::state {
    public:
        controller(core::context* context, const std::string& name);

        ~controller() override;

    protected:
        void on_draw() override;

        void on_resize() override;

        void on_initialize() override;

        void on_update(uint32_t dt) override;

        bool on_process_event(const SDL_Event* e) override;

    private:
    };

};

