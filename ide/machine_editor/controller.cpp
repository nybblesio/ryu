//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <rttr/type>
#include <core/engine.h>
#include <ide/ide_types.h>
#include <hardware/hardware.h>
#include <hardware/registry.h>
#include <core/input_action.h>
#include "controller.h"

namespace ryu::ide::machine_editor {

    static logger* s_log = logger_factory::instance()->create(
            "machine_editor::controller",
            logger::level::info);

    controller::controller(const std::string& name) : core::state(name) {
    }

    void controller::define_actions() {
        auto leave_action = core::input_action::create_no_map(
                "machine_editor_leave",
                "Internal",
                "Close the machine editor and return to previous state.");
        if (!leave_action->has_bindings())
            leave_action->bind_keys({core::key_escape});

        auto add_action = core::input_action::create_no_map(
                "machine_editor_add",
                "Internal",
                "Add a new component to the machine and open the editor.");
        if (!add_action->has_bindings())
            add_action->bind_keys({core::key_f1});

        auto edit_action = core::input_action::create_no_map(
                "machine_editor_edit",
                "Internal",
                "Edit the selected component.");
        if (!edit_action->has_bindings())
            edit_action->bind_keys({core::key_return});

        auto map_action = core::input_action::create_no_map(
                "machine_editor_map",
                "Internal",
                "Show the memory map of this machine.");
        if (!map_action->has_bindings())
            map_action->bind_keys({core::key_f2});

        auto delete_action = core::input_action::create_no_map(
                "machine_editor_delete",
                "Internal",
                "Remove the selected component from this machine.");
        if (!delete_action->has_bindings())
            delete_action->bind_keys({core::key_delete});
    }

    void controller::bind_events() {
        action_provider().register_handler(
                core::input_action::find_by_name("machine_editor_leave"),
                [this](const core::event_data_t& data) {
                    end_state();
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("machine_editor_add"),
                [](const core::event_data_t& data) {
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("machine_editor_map"),
                [](const core::event_data_t& data) {
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("machine_editor_edit"),
                [this](const core::event_data_t& data) {
                    if (!_component_pick_list->focused() || _machine == nullptr)
                        return false;

                    auto key = _component_pick_list->rows()[_component_pick_list->selected()].key;
                    _component_pick_list->reset_search();
                    transition_to(
                        core::system_commands::edit_component,
                        {
                            {"machine-id", _machine->id()},
                            {"component-id", key}
                        });
                    return true;
                });
        action_provider().register_handler(
                core::input_action::find_by_name("machine_editor_delete"),
                [this](const core::event_data_t& data) {
                    if (!_component_pick_list->focused())
                        return false;
                    return true;
                });
    }

    void controller::on_deactivate() {
        _layout_panel->visible(false);
        _display_pick_list->reset_search();
        _component_pick_list->reset_search();
    }

    void controller::update_values() {
        _name_textbox->value("");
        _display_pick_list->value("");
        _address_space_textbox->value("");
        _description_text_editor->clear();
        _component_pick_list->clear_rows();

        if (_machine == nullptr)
            return;

        _name_textbox->value(_machine->name());
        auto display_instance = _machine->display();
        if (display_instance != nullptr)
            _display_pick_list->value(display_instance->name());
        _address_space_textbox->value(fmt::format(
            "{0:08x}",
            _machine->address_space()));

        for (auto component : _machine->components()) {
            _component_pick_list->add_row({
                component->id(),
                {
                    ryu::core::pick_list_column_t(component->id()),
                    ryu::core::pick_list_column_t(component->name()),
                    ryu::core::pick_list_column_t(component->address()),
                    ryu::core::pick_list_column_t(component->ic()->name())
                }
            });
        }
    }

    void controller::on_initialize() {
        define_actions();
        bind_events();
    }

    hardware::machine* controller::machine() {
        return _machine;
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
                "assets/views/machine-editor.yaml");
        s_log->result(result);
        if (result.is_failed())
            return false;

        _name_textbox = _layout_panel->find_by_name<core::text_box>("name-text-box");
        _display_pick_list = _layout_panel->find_by_name<core::pick_list>("display-pick-list");
        _component_pick_list = _layout_panel->find_by_name<core::column_pick_list>("pick-list");
        _address_space_textbox = _layout_panel->find_by_name<core::text_box>("address-space-text-box");
        _description_text_editor = _layout_panel->find_by_name<core::text_editor>("description-text-editor");

        return !result.is_failed();
    }

    void controller::on_draw(core::renderer& surface) {
        _layout_panel->draw(surface);
    }

    void controller::machine(hardware::machine* value) {
        _machine = value;
        update_values();
    }

    void controller::on_resize(const core::rect& bounds) {
        layout_engine()->size(bounds.to_size());
    }

    void controller::on_activate(const core::parameter_dict& params) {
        _layout_panel->visible(true);

        auto it = params.find("name");
        if (it != params.end()) {
            auto name_param = boost::get<std::string>(it->second);
            auto mach = hardware::registry::instance()->find_machine(name_param);
            if (mach == nullptr) {
                mach = hardware::registry::instance()->new_machine();
                mach->name(name_param);
            }
            machine(mach);
            context()->resize();
        } else {
            update_values();
        }
    }

    void controller::on_update(uint32_t dt, core::pending_event_list& events) {
        _layout_panel->update(dt, events);
    }

}