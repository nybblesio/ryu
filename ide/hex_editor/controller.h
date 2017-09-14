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
#include <core/caret.h>

namespace ryu::ide::hex_editor {

    class controller : public core::state {
    public:
        enum ids {
        };

        controller(
                core::context* context,
                int id,
                const std::string& name);

    protected:
        void on_update(uint32_t dt) override;

        void on_draw(SDL_Renderer* renderer) override;

        void on_init(SDL_Renderer* renderer) override;

        bool on_process_event(const SDL_Event* e) override;

    private:
    };

};

