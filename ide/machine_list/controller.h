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
#include <core/view_factory.h>

namespace ryu::ide::machine_list {

    class controller : public ryu::core::state {
    public:
        explicit controller(const std::string& name);

    protected:
        struct metrics_t {
            const int left_padding = 10;
            const int right_padding = 50;
        };

        void on_initialize() override;

        void on_deactivate() override;

        void on_update(uint32_t dt) override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const core::rect& bounds) override;

        void on_activate(const core::parameter_dict& params) override;

    private:
        void bind_events();

        void create_views();

        void edit_new_machine();

        void edit_selected_machine();

        void delete_selected_machine();

    private:
        metrics_t _metrics;
        core::button_unique_ptr _add_button;
        core::button_unique_ptr _edit_button;
        core::state_header_unique_ptr _header;
        core::button_unique_ptr _delete_button;
        core::column_pick_list_unique_ptr _pick_list;
        core::dock_layout_panel_unique_ptr _layout_panel;
        core::dock_layout_panel_unique_ptr _buttons_panel;
    };

};

