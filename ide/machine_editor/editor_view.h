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
#include <core/views/panel.h>

namespace ryu::ide::machine_editor {

    class editor_view : public core::view {
    public:
        editor_view(
                core::context* context,
                core::view* parent,
                const std::string& name);

        ~editor_view() override;

        void initialize(hardware::machine* mach);

    protected:
        struct metrics_t {
            const int left_padding = 10;
            const int right_padding = 10;
        };

        void on_draw() override;

        void on_focus_changed() override;

        bool on_process_event(const SDL_Event* e) override;

    private:
        metrics_t _metrics;
        core::label _header;
        core::label _footer;
        core::panel _row1_panel;
        core::panel _row2_panel;
        core::label _name_label;
        core::textbox _name_textbox;
        core::label _address_space_label;
        core::textbox _address_space_textbox;
        core::label _display_label;
        hardware::machine* _machine = nullptr;
    };

};

