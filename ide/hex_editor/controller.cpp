//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <core/engine.h>
#include <core/project.h>
#include <ide/ide_context.h>
#include <core/input_action.h>
#include <core/project_file.h>
#include "controller.h"

namespace ryu::ide::hex_editor {

    static logger* s_log = logger_factory::instance()->create(
            "hex_editor::controller",
            logger::level::info);

    controller::controller(const std::string& name) : core::state(name) {
    }

    void controller::define_actions() {
        auto leave_action = core::input_action::create_no_map(
                "memory_editor_leave",
                "Internal",
                "Close the memory editor and return to previous state.");
        if (!leave_action->has_bindings())
            leave_action->bind_keys({core::key_escape});

        auto command_bar_action = core::input_action::create_no_map(
                "memory_editor_command_bar",
                "Internal",
                "Activate the command bar.");
        if (!command_bar_action->has_bindings())
            command_bar_action->bind_keys({core::mod_ctrl, core::key_space});
    }

    void controller::bind_events() {
        action_provider().register_handler(
            core::input_action::find_by_name("memory_editor_leave"),
            [this](const core::event_data_t& data) {
                end_state();
                return true;
            });
        action_provider().register_handler(
            core::input_action::find_by_name("memory_editor_command_bar"),
            [this](const core::event_data_t& data) {
                _layout_panel->focus(_command_line);
                return true;
            });
    }

    void controller::on_deactivate() {
        _layout_panel->visible(false);
        core::project::instance()
                ->machine()
                ->set_write_latches(false);
    }

    void controller::on_initialize() {
        define_actions();
        bind_events();
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
                "assets/views/hex-editor.yaml");
        s_log->result(result);

        _memory_editor = _layout_panel->find_by_name<core::memory_editor>("memory-editor");
        _command_line = _layout_panel->find_by_name<core::text_box>("command-line-text-box");

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
        core::project::instance()
                ->machine()
                ->set_write_latches(true);

        auto it = params.find("addr");
        if (it != params.end()) {
            _memory_editor->address(boost::get<uint32_t>(it->second));
        }
    }

    void controller::on_update(uint32_t dt, core::pending_event_list& events) {
        _layout_panel->update(dt, events);
    }

}