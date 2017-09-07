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

#include <core/textbox.h>
#include <core/label.h>
#include <core/result.h>

namespace ryu::ide::machine_editor {

    class editor_view : public core::view {
    public:
        enum ids {
            name_label = 1,
            name_textbox = 2,
        };

        editor_view(
                core::context* context,
                core::view* parent,
                int id,
                const std::string& name);

        ~editor_view() override;

    protected:
        void on_draw(SDL_Renderer* renderer) override;

        bool on_process_event(const SDL_Event* e) override;

    private:
        core::label _name_label;
        core::textbox _name_textbox;
    };

};

