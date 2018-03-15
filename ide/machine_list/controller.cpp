//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <ide/ide_types.h>
#include <core/input_action.h>
#include <core/view_factory.h>
#include <core/views/dock_layout_panel.h>
#include "controller.h"

namespace ryu::ide::machine_list {

    controller::controller(const std::string& name) : ryu::core::state(name) {
    }

    void controller::bind_events() {
        auto leave_action = core::input_action::create_no_map(
            "machine_list_leave",
            "Internal",
            "Close the machine list and return to previous state.");
        leave_action->register_handler(
            core::action_sink::controller,
            [this](const core::event_data_t& data) {
                return is_focused();
            },
            [this](const core::event_data_t& data) {
                end_state();
                return true;
            });
        leave_action->bind_keys({core::key_escape});
    }

    void controller::create_views() {
        _header = core::view_factory::create_state_header(
            this,
            "header-panel",
            ide::colors::info_text,
            ide::colors::fill_color,
            core::dock::styles::top,
            {_metrics.left_padding, _metrics.right_padding, 5, 15});
        _header->state("machine list");
        _header->state_color(ide::colors::white);

        _layout_panel = core::view_factory::create_dock_layout_panel(
            this,
            "layout-panel",
            ide::colors::info_text,
            ide::colors::fill_color);
        _layout_panel->add_child(_header.get());
    }

    void controller::on_initialize() {
        bind_events();
        create_views();
    }

    void controller::on_deactivate() {
    }

    void controller::on_update(uint32_t dt) {
    }

    void controller::on_draw(core::renderer& surface) {
        _layout_panel->draw(surface);
    }

    void controller::on_resize(const core::rect& bounds) {
        _layout_panel->resize(bounds);
    }

    void controller::on_activate(const core::parameter_dict& params) {
    }

}