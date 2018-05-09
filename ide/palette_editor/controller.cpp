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

namespace ryu::ide::palette_editor {

    static logger* s_log = logger_factory::instance()->create(
        "palette_editor::controller",
        logger::level::info);

    controller::controller(const std::string& name) : core::state(name) {
    }

    void controller::bind_events() {
        action_provider().register_handler(
            core::input_action::find_by_name("palette_editor_leave"),
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
            "palette_editor_leave",
            "Internal",
            "Close the palette editor and return to previous state.");
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
            "assets/views/palette-editor.yaml");

        s_log->result(result);
        if (result.is_failed())
            return false;

        _palettes = _layout_panel->find_by_name<core::list_box>("palette-list-box");
        _editor = _layout_panel->find_by_name<core::palette_editor>("editor");

//        _palettes->on_clicked([this]() {
//            // XXX: update palette on _editor
//        });

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

        auto project = core::project::instance();
        if (project == nullptr || project->machine() == nullptr) {
            return;
        }

        _video_generator = project->machine()->video_generator();
        if (_video_generator == nullptr) {
            s_log->warn("machine doesn't have an integrated circuit that supports core::video_generator.");
            return;
        }

        auto palette_attributes = _video_generator->palette_attributes();
        // XXX: need to convert contents of memory pointed to by video_generator
        //      into n palette instances with entries.

        auto& options = _palettes->options();
        options.clear();

        for (uint32_t i = 0; i < palette_attributes.number_of_palettes; i++) {
            options.push_back(core::pick_list_option_t {i, fmt::format("palette {}", i)});
        }
    }

    void controller::on_update(uint32_t dt, core::pending_event_list& events) {
        _layout_panel->update(dt, events);
    }

}