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

namespace ryu::ide::machine_editor {

    class controller : public ryu::core::state {
    public:
        explicit controller(const std::string& name);

        hardware::machine* machine();

        void machine(hardware::machine* value);

    protected:
        struct metrics_t {
            const int row_panel_margin = 15;
            const int button_panel_margin_top = 5;
            const int button_panel_margin_bottom = 5;

            const int left_padding = 10;
            const int right_padding = 50;
        };

        void on_deactivate() override;

        void on_initialize() override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const core::rect& bounds) override;

        void on_activate(const core::parameter_dict& params) override;

        void on_update(uint32_t dt, core::pending_event_list& events) override;

    private:
        void bind_events();

        void update_values();

        void define_actions();

    private:
        metrics_t _metrics;
        hardware::machine* _machine;
        core::textbox* _name_textbox;
        core::pick_list* _display_pick_list;
        core::textbox* _address_space_textbox;
        core::text_editor* _description_text_editor;
        core::column_pick_list* _component_pick_list;
        core::loadable_view_unique_ptr _layout_panel;
    };

};

