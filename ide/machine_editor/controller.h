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
#include <core/views/notebook.h>
#include <core/views/dock_layout_panel.h>
#include "general_tab.h"

namespace ryu::ide::machine_editor {

    class controller : public ryu::core::state {
    public:
        explicit controller(const std::string& name);

        hardware::machine* machine();

        void machine(hardware::machine* value);

    protected:
        struct metrics_t {
            const int left_padding = 10;
            const int right_padding = 10;
        };

        void on_deactivate() override;

        void on_initialize() override;

        void on_update(uint32_t dt) override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const core::rect& bounds) override;

        void on_activate(const core::parameter_dict& params) override;

    private:
        void update_values();

    private:
        metrics_t _metrics;
        core::label _header;
        core::label _footer;
        core::label _name_label;
        core::notebook _notebook;
        core::button _map_button;
        core::button _add_button;
        core::label _display_label;
        core::button _delete_button;
        core::textbox _name_textbox;
        hardware::machine* _machine;
        core::dock_layout_panel _panel;
        core::label _address_space_label;
        core::pick_list _display_pick_list;
        core::dock_layout_panel _row1_panel;
        core::dock_layout_panel _row2_panel;
        core::dock_layout_panel _row3_panel;
        core::textbox _address_space_textbox;
        core::dock_layout_panel _button_panel;
        core::dock_layout_panel _general_panel;
    };

};

