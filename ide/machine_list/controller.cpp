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

    void controller::create_views() {
        _header = core::view_factory::create_view<core::state_header>(
            this,
            "header-panel",
            context()->font_family(),
            &context()->palette(),
            ide::colors::info_text,
            ide::colors::fill_color,
            "",
            core::dock::styles::top,
            {_metrics.left_padding, _metrics.right_padding, 5, 15});
        _header->state("machine list");
        _header->state_color(ide::colors::white);

        _pick_list = core::view_factory::create_view<core::column_pick_list>(
                this,
                "pick-list",
                context()->font_family(),
                &context()->palette(),
                ide::colors::info_text,
                ide::colors::fill_color,
                "",
                core::dock::styles::fill,
                {_metrics.left_padding, _metrics.right_padding, 5, 15});
        _pick_list->border(core::border::types::solid);
        _pick_list->add_header(
                "ID",
                ide::colors::white,
                ide::colors::light_blue,
                125,
                core::column_pick_list::halign_t::left);
        _pick_list->add_header(
                "Name",
                ide::colors::white,
                ide::colors::light_blue,
                525,
                core::column_pick_list::halign_t::left);
        _pick_list->add_header(
                "Address Space",
                ide::colors::white,
                ide::colors::light_blue,
                250,
                core::column_pick_list::halign_t::left);
        _pick_list->add_header(
                "Display",
                ide::colors::white,
                ide::colors::light_blue,
                525,
                core::column_pick_list::halign_t::left);
        _pick_list->on_activated([this](uint32_t row) {
            edit_selected_machine();
        });
        _pick_list->on_selection_changed([this](int32_t row) {
            _edit_button->enabled(row != -1);
            _delete_button->enabled(row != -1);
        });

        _add_button = core::view_factory::create_view<core::button>(
            this,
            "add-button",
            context()->font_family(),
            &context()->palette(),
            ide::colors::light_grey,
            ide::colors::light_blue,
            "Add (F1)",
            core::dock::styles::left,
            {5, 5, 5, 5});
        _add_button->width(230);
        _add_button->on_clicked([this]() {
            edit_new_machine();
        });

        _edit_button = core::view_factory::create_view<core::button>(
            this,
            "edit-button",
            context()->font_family(),
            &context()->palette(),
            ide::colors::light_grey,
            ide::colors::light_blue,
            "Edit (RETURN)",
            core::dock::styles::left,
            {5, 5, 5, 5});
        _edit_button->width(230);
        _edit_button->enabled(false);
        _edit_button->on_clicked([this]() {
            edit_selected_machine();
        });

        _delete_button = core::view_factory::create_view<core::button>(
            this,
            "delete-button",
            context()->font_family(),
            &context()->palette(),
            ide::colors::light_grey,
            ide::colors::light_blue,
            "Delete (DEL)",
            core::dock::styles::left,
            {5, 5, 5, 5});
        _delete_button->width(230);
        _delete_button->enabled(false);
        _delete_button->on_clicked([this]() {
            delete_selected_machine();
        });

        _buttons_panel = core::view_factory::create_view<core::dock_layout_panel>(
            this,
            "buttons-panel",
            context()->font_family(),
            &context()->palette(),
            ide::colors::info_text,
            ide::colors::fill_color,
            "",
            core::dock::styles::bottom,
            {_metrics.left_padding, _metrics.right_padding, 10, 10});
        _buttons_panel->bounds().height(_add_button->height());
        _buttons_panel->add_child(_add_button.get());
        _buttons_panel->add_child(_edit_button.get());
        _buttons_panel->add_child(_delete_button.get());

        _layout_panel = core::view_factory::create_view<core::dock_layout_panel>(
            this,
            "layout-panel",
            context()->font_family(),
            &context()->palette(),
            ide::colors::info_text,
            ide::colors::fill_color);
        _layout_panel->add_child(_header.get());
        _layout_panel->add_child(_buttons_panel.get());
        _layout_panel->add_child(_pick_list.get());

        _pick_list->next_view(_add_button.get());
        _pick_list->prev_view(_delete_button.get());

        _add_button->next_view(_edit_button.get());
        _add_button->prev_view(_pick_list.get());

        _edit_button->next_view(_delete_button.get());
        _edit_button->prev_view(_add_button.get());

        _delete_button->next_view(_pick_list.get());
        _delete_button->prev_view(_edit_button.get());

        _layout_panel->focus(_pick_list.get());
    }

    void controller::on_initialize() {
        define_actions();
        bind_events();
        create_views();
    }

    void controller::on_deactivate() {
        _layout_panel->visible(false);
    }

    void controller::edit_new_machine() {
        auto new_machine_name = fmt::format(
            "Machine {}",
            hardware::registry::instance()->machines().size());
        transition_to(
            "edit_machine",
            {{"name", new_machine_name}});
    }

    void controller::edit_selected_machine() {
        auto machine = hardware::registry::instance()
            ->find_machine(_pick_list->rows()[_pick_list->selected()].key);
        if (machine == nullptr)
            return;
        transition_to(
            "edit_machine",
            {{"name", machine->name()}});
    }

    void controller::delete_selected_machine() {
        core::result result;
        hardware::registry::instance()
            ->remove_machine(result, _pick_list->rows()[_pick_list->selected()].key);
        if (!result.is_failed())
            _pick_list->remove_row(_pick_list->selected());
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