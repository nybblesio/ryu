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

    controller::controller(const std::string& name) : core::state(name) {
    }

    void controller::bind_events() {
        auto leave_action = core::input_action::create_no_map(
            "machine_editor_leave",
            "Internal",
            "Close the machine editor and return to previous state.");
        leave_action->register_handler(
            core::action_sink::controller,
            core::action_sink::default_filter,
            [this](const core::event_data_t& data) {
                end_state();
                return true;
            });
        leave_action->bind_keys({core::key_escape});

        auto add_action = core::input_action::create_no_map(
            "machine_editor_add",
            "Internal",
            "Add a new component to the machine and open the editor.");
        add_action->register_handler(
            core::action_sink::controller,
            core::action_sink::default_filter,
            [this](const core::event_data_t& data) {
                return true;
            });
        add_action->bind_keys({core::key_f1});

        auto map_action = core::input_action::create_no_map(
            "machine_editor_map",
            "Internal",
            "Show the memory map of this machine.");
        map_action->register_handler(
            core::action_sink::controller,
            core::action_sink::default_filter,
            [this](const core::event_data_t& data) {
                return true;
            });
        map_action->bind_keys({core::key_f2});

        auto delete_action = core::input_action::create_no_map(
            "machine_editor_delete",
            "Internal",
            "Remove the selected component from this machine.");
        delete_action->register_handler(
            core::action_sink::controller,
            core::action_sink::default_filter,
            [this](const core::event_data_t& data) {
                return true;
            });
        delete_action->bind_keys({core::key_delete});
    }

    void controller::create_views() {
        _header = core::view_factory::create_state_header(
            this,
            "header-panel",
            ide::colors::info_text,
            ide::colors::fill_color,
            core::dock::styles::top,
            {_metrics.left_padding, _metrics.right_padding, 5, 15});
        _header->state("machine editor");
        _header->state_color(ide::colors::white);

        create_top_panel();
        create_button_panel();
        create_component_pick_list();

        _layout_panel = core::view_factory::create_dock_layout_panel(
            this,
            "panel",
            ide::colors::info_text,
            ide::colors::fill_color,
            core::dock::styles::fill);
        _layout_panel->add_child(_header.get());
        _layout_panel->add_child(_button_panel.get());
        _layout_panel->add_child(_top_panel.get());
        _layout_panel->add_child(_components_panel.get());

        _layout_panel->focus(_name_textbox.get());
    }

    void controller::on_deactivate() {
        _machine = nullptr;
    }

    void controller::update_values() {
        if (_machine == nullptr) {
            _name_textbox->value("");
            _display_pick_list->value("");
            _address_space_textbox->value("");
            _description_text_editor->clear();
            _component_pick_list->clear_rows();
            return;
        }

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
                    fmt::format("{}", component->id()),
                    component->name(),
                    fmt::format("{:08x}", component->address()),
                    component->ic()->name()
                }
            });
        }
    }

    void controller::on_initialize() {
        bind_events();
        create_views();
    }

    void controller::create_top_panel() {
        auto font_face = context()->font_face();
        auto display_label_width = font_face->measure_text("display:");
        auto address_space_label_width = font_face->measure_text("address space: $");

        _name_label = core::view_factory::create_label(
            this,
            "name-label",
            ide::colors::text,
            ide::colors::fill_color,
            "name:",
            core::dock::styles::left);
        _name_label->sizing(core::view::sizing::types::fixed);
        _name_label->halign(core::alignment::horizontal::right);
        _name_label->valign(core::alignment::vertical::middle);
        _name_label->bounds().size(
            display_label_width,
            font_face->line_height + 10);

        _name_textbox = core::view_factory::create_textbox(
            this,
            "name-textbox",
            ide::colors::text,
            ide::colors::fill_color,
            "",
            core::dock::styles::left,
            {20, 0, 0, 0});
        _name_textbox->width(32);
        _name_textbox->length(32);
        _name_textbox->enabled(true);
        _name_textbox->on_tab([&]() -> const core::view* {
            return _address_space_textbox.get();
        });
        _name_textbox->on_key_down([&](int key_code) {
            return true;
        });

        _address_space_label = core::view_factory::create_label(
            this,
            "address-space-label",
            ide::colors::text,
            ide::colors::fill_color,
            "address space: $",
            core::dock::styles::left,
            {20, 0, 0, 0});
        _address_space_label->sizing(core::view::sizing::types::fixed);
        _address_space_label->valign(core::alignment::vertical::middle);
        _address_space_label->bounds().size(
            address_space_label_width,
            font_face->line_height + 20);

        _address_space_textbox = core::view_factory::create_textbox(
            this,
            "address-space-textbox",
            ide::colors::text,
            ide::colors::fill_color,
            "",
            core::dock::styles::left,
            {20, 0, 0, 0});
        _address_space_textbox->width(8);
        _address_space_textbox->length(8);
        _address_space_textbox->enabled(true);
        _address_space_textbox->on_tab([&]() -> const core::view* {
            return _display_pick_list.get();
        });
        _address_space_textbox->on_key_down([](int key_code) {
            return isxdigit(key_code);
        });

        core::option_list display_options {};
        for (auto& display : hardware::display::catalog())
            display_options.push_back(display.name());

        _display_label = core::view_factory::create_label(
            this,
            "display-label",
            ide::colors::text,
            ide::colors::fill_color,
            "display:",
            core::dock::styles::left);
        _display_label->sizing(core::view::sizing::types::fixed);
        _display_label->halign(core::alignment::horizontal::right);
        _display_label->valign(core::alignment::vertical::middle);
        _display_label->bounds().size(display_label_width, font_face->line_height + 10);

        _display_pick_list = core::view_factory::create_pick_list(
            this,
            "display-pick-list",
            ide::colors::text,
            ide::colors::fill_color,
            display_options,
            core::dock::styles::left,
            {20, 0, 0, 0});
        _display_pick_list->on_tab([&]() -> const core::view* {
            return _description_text_editor.get();
        });

        auto row_margins = core::padding {
            _metrics.left_padding,
            _metrics.right_padding,
            _metrics.row_panel_margin,
            _metrics.row_panel_margin
        };
        _row1_panel = core::view_factory::create_dock_layout_panel(
            this,
            "row1-panel",
            ide::colors::info_text,
            ide::colors::transparent,
            core::dock::styles::top,
            row_margins);
        _row1_panel->bounds().height(font_face->line_height + 10);
        _row1_panel->add_child(_name_label.get());
        _row1_panel->add_child(_name_textbox.get());
        _row1_panel->add_child(_address_space_label.get());
        _row1_panel->add_child(_address_space_textbox.get());

        _row2_panel = core::view_factory::create_dock_layout_panel(
            this,
            "row2-panel",
            ide::colors::info_text,
            ide::colors::transparent,
            core::dock::styles::top,
            row_margins);
        _row2_panel->bounds().height(font_face->line_height + 10);
        _row2_panel->add_child(_display_label.get());
        _row2_panel->add_child(_display_pick_list.get());

        _description_text_editor = core::view_factory::create_text_editor(
            this,
            "description-text-editor",
            ide::colors::text,
            ide::colors::fill_color,
            64,
            80);
        _description_text_editor->caret_color(ide::colors::caret);
        _description_text_editor->selection_color(ide::colors::selection);
        _description_text_editor->line_number_color(ide::colors::info_text);

        _row3_panel = core::view_factory::create_dock_layout_panel(
            this,
            "row3-panel",
            ide::colors::info_text,
            ide::colors::transparent,
            core::dock::styles::top,
            row_margins);
        _row3_panel->bounds().height((font_face->line_height * 6) + 10);
        _row3_panel->add_child(_description_text_editor.get());

        _top_panel = core::view_factory::create_dock_layout_panel(
            this,
            "top-panel",
            ide::colors::info_text,
            ide::colors::fill_color,
            core::dock::styles::top);
        _top_panel->add_child(_row1_panel.get());
        _top_panel->add_child(_row2_panel.get());
        _top_panel->add_child(_row3_panel.get());
    }

    void controller::create_button_panel() {
        _add_button = core::view_factory::create_button(
            this,
            "add-button",
            ide::colors::light_grey,
            ide::colors::light_blue,
            "Add (F1)",
            core::dock::styles::left,
            {5, 5, 5, 5});
        _add_button->width(230);
        _add_button->on_tab([&]() -> const core::view* {
            return _edit_button.get();
        });

        _edit_button = core::view_factory::create_button(
            this,
            "edit-button",
            ide::colors::light_grey,
            ide::colors::light_blue,
            "Edit (RETURN)",
            core::dock::styles::left,
            {5, 5, 5, 5});
        _edit_button->width(230);
        _edit_button->on_tab([&]() -> const core::view* {
            return _delete_button.get();
        });

        _delete_button = core::view_factory::create_button(
            this,
            "delete-button",
            ide::colors::light_grey,
            ide::colors::light_blue,
            "Delete (DEL)",
            core::dock::styles::left,
            {5, 5, 5, 5});
        _delete_button->width(230);
        _delete_button->on_tab([&]() -> const core::view* {
            return _map_button.get();
        });

        _map_button = core::view_factory::create_button(
            this,
            "map-button",
            ide::colors::light_grey,
            ide::colors::light_blue,
            "Map (F2)",
            core::dock::styles::right,
            {5, 5, 5, 5});
        _map_button->width(230);
        _map_button->on_tab([&]() -> const core::view* {
            return _name_textbox.get();
        });

        auto button_panel_margin = core::padding {
            _metrics.left_padding,
            _metrics.right_padding,
            _metrics.button_panel_margin,
            _metrics.button_panel_margin
        };
        _button_panel = core::view_factory::create_dock_layout_panel(
            this,
            "button-panel",
            ide::colors::info_text,
            ide::colors::transparent,
            core::dock::styles::bottom,
            button_panel_margin);
        _button_panel->bounds().height(_map_button->height());
        _button_panel->add_child(_add_button.get());
        _button_panel->add_child(_edit_button.get());
        _button_panel->add_child(_delete_button.get());
        _button_panel->add_child(_map_button.get());
    }

    void controller::on_update(uint32_t dt) {
    }

    hardware::machine* controller::machine() {
        return _machine;
    }

    void controller::create_component_pick_list() {
        _component_pick_list = core::view_factory::create_column_pick_list(
            this,
            "pick-list",
            ide::colors::info_text,
            ide::colors::fill_color,
            {_metrics.left_padding, _metrics.right_padding, 5, 15});
        _component_pick_list->border(core::border::types::solid);
        _component_pick_list->sizing(core::view::sizing::types::parent);
        _component_pick_list->add_header(
            "ID",
            ide::colors::white,
            ide::colors::light_blue,
            100,
            core::column_pick_list::halign_t::left);
        _component_pick_list->add_header(
            "Name",
            ide::colors::white,
            ide::colors::light_blue,
            600,
            core::column_pick_list::halign_t::left);
        _component_pick_list->add_header(
            "Address",
            ide::colors::white,
            ide::colors::light_blue,
            150,
            core::column_pick_list::halign_t::center);
        _component_pick_list->add_header(
            "Integrated Circuit",
            ide::colors::white,
            ide::colors::light_blue,
            325,
            core::column_pick_list::halign_t::left);
        _component_pick_list->on_activated([this](uint32_t row) {
        });
        _component_pick_list->on_tab([&]() -> const core::view* {
            return _add_button.get();
        });
        _component_pick_list->on_selection_changed([this](int32_t row) {
        });

        auto row_margins = core::padding {
            _metrics.left_padding,
            _metrics.right_padding,
            _metrics.row_panel_margin,
            _metrics.row_panel_margin
        };
        _components_panel = core::view_factory::create_dock_layout_panel(
            this,
            "component-panel",
            ide::colors::info_text,
            ide::colors::transparent,
            core::dock::styles::fill,
            row_margins);
//        auto font_face = context()->font_face();
//        _components_panel->bounds().height((font_face->line_height * 6) + 10);
        _components_panel->add_child(_component_pick_list.get());
    }

    void controller::on_draw(core::renderer& surface) {
        _layout_panel->draw(surface);
    }

    void controller::machine(hardware::machine* value) {
        _machine = value;
        update_values();
    }

    void controller::on_resize(const core::rect& bounds) {
        _layout_panel->resize(bounds);
    }

    void controller::on_activate(const core::parameter_dict& params) {
        auto it = params.find("name");
        if (it != params.end()) {
            auto name_param = boost::get<std::string>(it->second);
            auto mach = hardware::registry::instance()->find_machine(name_param);
            if (mach == nullptr) {
                mach = hardware::registry::instance()->new_machine();
                mach->name(name_param);
            }
            machine(mach);
        }
    }

}