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
        void on_initialize() override;

        void on_deactivate() override;

        bool on_load(core::result& result) override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const core::rect& bounds) override;

        void on_activate(const core::parameter_dict& params) override;

        void on_update(uint32_t dt, core::pending_event_list& events) override;

    private:
        void bind_events();

        void define_actions();

        void edit_new_machine();

        void edit_selected_machine();

        void delete_selected_machine();

    private:
        core::button* _add_button;
        core::button* _edit_button;
        core::button* _delete_button;
        core::column_pick_list* _pick_list;
        core::dock_layout_panel_unique_ptr _layout_panel;
    };

};

