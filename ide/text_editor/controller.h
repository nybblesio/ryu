//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <core/caret.h>
#include <core/state.h>
#include <common/SDL_FontCache.h>
#include "editor_view.h"
#include "core/document.h"

namespace ryu::ide::text_editor {

    class controller : public ryu::core::state {
    public:
        const int rows = 4096;
        const int columns = 128;

        enum ids {
            text_editor = 1,
        };

        controller(
                core::context* context,
                int id,
                const std::string& name);

        ~controller() override;

    protected:
        void on_update(uint32_t dt) override;

        void on_draw(SDL_Renderer* renderer) override;

        void on_init(SDL_Renderer* renderer) override;

        bool on_process_event(const SDL_Event* e) override;

    private:
        editor_view _view;
        command_factory _command_factory;
    };

};
