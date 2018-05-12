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
#include <logger_factory.h>
#include "controller.h"

namespace ryu::ide::project_editor {

    static logger* s_log = logger_factory::instance()->create(
        "project_editor::controller",
        logger::level::info);

    controller::controller(const std::string& name) : core::state(name) {
    }

    void controller::bind_events() {
        action_provider().register_handler(
            core::input_action::find_by_name("project_editor_leave"),
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
        _file_pick_list->clear_rows();
        _name_text_box->value("");
        _machine_pick_list->value("");
        _description_text_editor->clear();

        auto project = core::project::instance();
        if (project == nullptr)
            return;

        _name_text_box->value(project->name());
        auto machine_instance = project->machine();
        if (machine_instance != nullptr)
            _machine_pick_list->selected_key(machine_instance->id());

        auto index = 0;
        auto project_files = project->files();
        for (const auto& file : project_files) {
            auto type_code = core::project_file_type::type_to_code(file->type());
            to_upper(type_code);
            _file_pick_list->add_row({
                    file->id(),
                    {
                        ryu::core::pick_list_column_t(file->id()),
                        ryu::core::pick_list_column_t(file->name()),
                        ryu::core::pick_list_column_t(type_code),
                        ryu::core::pick_list_column_t(file->should_assemble()),
                        ryu::core::pick_list_column_t(index > 0 ? "Up" : ""),
                        ryu::core::pick_list_column_t(index < project_files.size() - 1 ? "Down" : "")
                    }
                });
            ++index;
        }
    }

    void controller::define_actions() {
        auto leave_action = core::input_action::create_no_map(
            "project_editor_leave",
            "Internal",
            "Close the project editor and return to previous state.");
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
            "assets/views/project-editor.yaml");

        s_log->result(result);
        if (result.is_failed())
            return false;

        _add_button = _layout_panel->find_by_name<core::button>("add-button");
        _edit_button = _layout_panel->find_by_name<core::button>("edit-button");
        _delete_button = _layout_panel->find_by_name<core::button>("delete-button");
        _name_text_box = _layout_panel->find_by_name<core::text_box>("name-text-box");
        _file_pick_list = _layout_panel->find_by_name<core::column_pick_list>("pick-list");
        _machine_pick_list = _layout_panel->find_by_name<core::pick_list>("machine-pick-list");
        _description_text_editor = _layout_panel->find_by_name<core::text_editor>("description-text-editor");

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
        update_values();
    }

    void controller::on_update(uint32_t dt, core::pending_event_list& events) {
        _layout_panel->update(dt, events);
    }

};