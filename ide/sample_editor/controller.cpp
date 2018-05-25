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
#include "controller.h"

namespace ryu::ide::sample_editor {

    static logger* s_log = logger_factory::instance()->create(
        "sample_editor::controller",
        logger::level::info);

    controller::controller(const std::string& name) : core::state(name) {
    }

    void controller::bind_events() {
        action_provider().register_handler(
            core::input_action::find_by_name("sample_editor_leave"),
            [this](const core::event_data_t& data) {
                end_state();
                return true;
            });
    }

    void controller::on_deactivate() {
        _layout_panel->visible(false);
    }

    void controller::on_initialize() {
        define_actions();
        bind_events();
    }

    void controller::update_values() {
    }

    void controller::define_actions() {
        auto leave_action = core::input_action::create_no_map(
            "sample_editor_leave",
            "Internal",
            "Close the sample editor and return to previous state.");
        if (!leave_action->has_bindings())
            leave_action->bind_keys({core::key_escape});
    }

    bool controller::on_load(core::result& result) {
        _layout_panel = core::view_factory::create_loadable_view(
            this,
            "loadable-view",
            context()->font_family(),
            &context()->palette(),
            context()->prefs(),
            ide::colors::info_text,
            ide::colors::fill_color,
            result,
            "assets/views/sample-editor.yaml");

        s_log->result(result);
        if (result.is_failed())
            return false;

        return !result.is_failed();
    }

    void controller::on_draw(core::renderer& surface) {
        _layout_panel->draw(surface);
    }

    void controller::on_resize(const core::rect& bounds) {
        layout_engine()->size(bounds.to_size());
    }

    void controller::on_activate(const core::parameter_dict& params) {
        _layout_panel->visible(true);
    }

    void controller::on_update(uint32_t dt, core::pending_event_list& events) {
        _layout_panel->update(dt, events);
    }

}