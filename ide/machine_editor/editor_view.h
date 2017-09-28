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

#include <core/result.h>
#include <core/views/label.h>
#include <core/views/textbox.h>
#include <hardware/machine.h>

namespace ryu::ide::machine_editor {

    class editor_view : public core::view {
    public:
        enum ids {
            name_label = 100,
            name_textbox = 101,
            display_label = 102
        };

        editor_view(
                core::context* context,
                core::view* parent,
                int id,
                const std::string& name);

        ~editor_view() override;

        void initialize(hardware::machine* mach);

    protected:
        void on_draw() override;

        void on_resize() override;

        void on_focus_changed() override;

        bool on_process_event(const SDL_Event* e) override;

    private:
        core::label _name_label;
        core::label _display_label;
        core::textbox _name_textbox;
        hardware::machine* _machine = nullptr;
    };

};

