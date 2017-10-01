//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <core/state.h>
#include <common/SDL_FontCache.h>
#include "editor_view.h"

namespace ryu::ide::text_editor {

    class controller : public ryu::core::state {
    public:
        const int rows = 4096;
        const int columns = 128;

        controller(core::context* context, const std::string& name);

        ~controller() override;

    protected:
        void on_draw() override;

        void on_resize() override;

        void on_initialize() override;

        void on_update(uint32_t dt) override;

        bool on_process_event(const SDL_Event* e) override;

    private:
        editor_view _view;
        bool _initialized = false;
        environment _command_factory;
    };

};
