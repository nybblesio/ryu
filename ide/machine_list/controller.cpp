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
#include <hardware/registry.h>
#include "controller.h"

namespace ryu::ide::machine_list {

    static logger* s_log = logger_factory::instance()->create(
        "machine_list::controller",
        logger::level::info);

    controller::controller(const std::string& name) : ryu::core::state(name) {
    }

    void controller::define_actions() {
        auto leave_action = core::input_action::create_no_map(
            "machine_list_leave",
            "Internal",
            "Close the machine list and return to previous state.");
        if (!leave_action->has_bindings())
            leave_action->bind_keys({core::key_escape});

        auto add_action = core::input_action::create_no_map(
            "machine_list_add",
            "Internal",
            "Add a new machine and open the editor.");
        if (!add_action->has_bindings())
            add_action->bind_keys({core::key_f1});

        auto delete_action = core::input_action::create_no_map(
            "machine_list_delete",
            "Internal",
            "Delete the selection machine from the registry.");
        if (!delete_action->has_bindings())
            delete_action->bind_keys({core::key_delete});
    }

    void controller::bind_events() {
        action_provider().register_handler(
            core::input_action::find_by_name("machine_list_leave"),
            [this](const core::event_data_t& data) {
                end_state();
                return true;
            });
        action_provider().register_handler(
            core::input_action::find_by_name("machine_list_add"),
            [this](const core::event_data_t& data) {
                edit_new_machine();
                return true;
            });
        action_provider().register_handler(
            core::input_action::find_by_name("machine_list_delete"),
            [this](const core::event_data_t& data) {
                delete_selected_machine();
                return true;
            });
    }

    void controller::on_initialize() {
        define_actions();
        bind_events();
    }

    void controller::on_deactivate() {
        _layout_panel->visible(false);
    }

    void controller::edit_new_machine() {
        auto new_machine_name = fmt::format(
            "Machine {}",
            hardware::registry::instance()->machines().size());
        transition_to(
            core::system_commands::edit_machine,
            {{"name", new_machine_name}});
    }

    void controller::edit_selected_machine() {
        auto machine = hardware::registry::instance()
            ->find_machine(_pick_list->rows()[_pick_list->selected()].key);
        if (machine == nullptr)
            return;
        transition_to(
            core::system_commands::edit_machine,
            {{"name", machine->name()}});
    }

    void controller::delete_selected_machine() {
        core::result result;
        hardware::registry::instance()
            ->remove_machine(result, _pick_list->rows()[_pick_list->selected()].key);
        if (!result.is_failed())
            _pick_list->remove_row(_pick_list->selected());
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
            "assets/views/machine-list.yaml");
        s_log->result(result);

        _add_button = _layout_panel->find_by_name<core::button>("add-button");
        _edit_button = _layout_panel->find_by_name<core::button>("edit-button");
        _delete_button = _layout_panel->find_by_name<core::button>("delete-button");
        _pick_list = _layout_panel->find_by_name<core::column_pick_list>("pick-list");

        _pick_list->on_activated([this](uint32_t row) {
            edit_selected_machine();
        });
        _pick_list->on_selection_changed([this](int32_t row) {
            _edit_button->enabled(row != -1);
            _delete_button->enabled(row != -1);
        });

        _add_button->on_clicked([this]() {
            edit_new_machine();
        });

        _edit_button->on_clicked([this]() {
            edit_selected_machine();
        });

        _delete_button->on_clicked([this]() {
            delete_selected_machine();
        });

        return !result.is_failed();
    }

    void controller::on_draw(core::renderer& surface) {
        _layout_panel->draw(surface);
    }

    void controller::on_resize(const core::rect& bounds) {
        _layout_panel->resize(bounds);
    }

    void controller::on_activate(const core::parameter_dict& params) {
        _pick_list->clear_rows();

        for (const auto machine : hardware::registry::instance()->machines()) {
            std::string display_name("(none)");
            if (machine->display() != nullptr)
                display_name = machine->display()->name();
            _pick_list->add_row({
                machine->id(),
                {
                    fmt::format("{}", machine->id()),
                    machine->name(),
                    fmt::format("{:08x}", machine->address_space()),
                    display_name
                }
            });
        }

        _layout_panel->visible(true);
        context()->resize();
    }

    void controller::on_update(uint32_t dt, core::pending_event_list& events) {
        _layout_panel->update(dt, events);
    }

}