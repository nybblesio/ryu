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
#include <core/view_factory.h>
#include <core/views/notebook.h>
#include <core/views/state_header.h>
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
        void bind_events();

        void update_values();

    private:
        metrics_t _metrics;
        hardware::machine* _machine;
        core::label_unique_ptr _footer;
        core::label_unique_ptr _name_label;
        core::notebook_unique_ptr _notebook;
        core::button_unique_ptr _map_button;
        core::button_unique_ptr _add_button;
        core::label_unique_ptr _display_label;
        core::state_header_unique_ptr _header;
        core::button_unique_ptr _delete_button;
        core::textbox_unique_ptr _name_textbox;
        core::dock_layout_panel_unique_ptr _panel;
        core::label_unique_ptr _address_space_label;
        core::pick_list_unique_ptr _display_pick_list;
        core::dock_layout_panel_unique_ptr _row1_panel;
        core::dock_layout_panel_unique_ptr _row2_panel;
        core::dock_layout_panel_unique_ptr _row3_panel;
        core::textbox_unique_ptr _address_space_textbox;
        core::dock_layout_panel_unique_ptr _button_panel;
        core::dock_layout_panel_unique_ptr _general_panel;
    };

};

