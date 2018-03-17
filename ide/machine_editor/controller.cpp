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

    void controller::on_deactivate() {
        _machine = nullptr;
    }

    void controller::update_values() {
        if (_machine != nullptr) {
            _name_textbox->value(_machine->name());
            auto disp = _machine->display();
            if (disp != nullptr)
                _display_pick_list->value(disp->name());
            _address_space_textbox->value(fmt::format("{0:08x}", _machine->address_space()));
        }
    }

    void controller::on_initialize() {
        bind_events();

        auto font_face = context()->font_face();
        auto display_label_width = font_face->measure_text("display:");
        auto address_space_label_width = font_face->measure_text("address space: $");

        _header = core::view_factory::create_state_header(
            this,
            "header-panel",
            ide::colors::info_text,
            ide::colors::fill_color,
            core::dock::styles::top,
            {_metrics.left_padding, _metrics.right_padding, 5, 15});
        _header->state("machine editor");
        _header->state_color(ide::colors::white);

        _name_label = core::view_factory::create_label(
            this,
            "name-label",
            ide::colors::text,
            ide::colors::fill_color,
            "name:",
            core::dock::styles::left,
            {5, 5, 5, 5});
        _name_label->border(core::border::types::solid);
        _name_label->sizing(core::view::sizing::types::fixed);
        _name_label->halign(core::alignment::horizontal::right);
        _name_label->valign(core::alignment::vertical::middle);
        _name_label->bounds().size(display_label_width, font_face->line_height);

        _name_textbox = core::view_factory::create_textbox(
            this,
            "name-textbox",
            ide::colors::text,
            ide::colors::fill_color,
            "",
            core::dock::styles::left,
            {5, 5, 5, 5});
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
            {20, 5, 5, 5});
        _address_space_label->border(core::border::types::solid);
        _address_space_label->sizing(core::view::sizing::types::fixed);
        _address_space_label->valign(core::alignment::vertical::middle);
        _address_space_label->bounds().size(
            address_space_label_width,
            font_face->line_height + 10);

        _address_space_textbox = core::view_factory::create_textbox(
            this,
            "address-space-textbox",
            ide::colors::text,
            ide::colors::fill_color,
            "",
            core::dock::styles::left,
            {5, 5, 5, 5});
        _address_space_textbox->width(8);
        _address_space_textbox->length(8);
        _address_space_textbox->enabled(true);
        _address_space_textbox->on_tab([&]() -> const core::view* {
            return _display_pick_list.get();
        });
        _address_space_textbox->on_key_down([](int key_code) {
            return isxdigit(key_code);
        });

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

        core::option_list display_options {};
        for (auto& display : hardware::display::catalog())
            display_options.push_back(display.name());

        _display_pick_list = core::view_factory::create_pick_list(
            this,
            "display-pick-list",
            ide::colors::text,
            ide::colors::fill_color,
            display_options,
            core::dock::styles::left,
            {15, 0, 4, 0});
        _display_pick_list->on_tab([&]() -> const core::view* {
            return _add_button.get();
        });
//        _display_pick_list->bounds().size(
//            context()->font_face()->width * _display_pick_list->length(),
//            context()->font_face()->line_height);

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

        _row1_panel = core::view_factory::create_dock_layout_panel(
            this,
            "row1-panel",
            ide::colors::info_text,
            ide::colors::transparent,
            core::dock::styles::top,
            {_metrics.left_padding, _metrics.right_padding, 20, 20});
        _row1_panel->bounds().height(context()->font_face()->line_height);
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
            {_metrics.left_padding, _metrics.right_padding, 20, 20});
        _row2_panel->bounds().height(context()->font_face()->line_height);
        _row2_panel->add_child(_display_label.get());
        _row2_panel->add_child(_display_pick_list.get());

        _button_panel = core::view_factory::create_dock_layout_panel(
            this,
            "button-panel",
            ide::colors::info_text,
            ide::colors::transparent,
            core::dock::styles::bottom,
            {_metrics.left_padding, _metrics.right_padding, 10, 10});
        _button_panel->bounds().height(_map_button->height());
        _button_panel->add_child(_map_button.get());
        _button_panel->add_child(_add_button.get());
        _button_panel->add_child(_delete_button.get());

        _top_panel = core::view_factory::create_dock_layout_panel(
            this,
            "top-panel",
            ide::colors::info_text,
            ide::colors::fill_color,
            core::dock::styles::fill);
        _top_panel->add_child(_row1_panel.get());
        _top_panel->add_child(_row2_panel.get());

        _panel = core::view_factory::create_dock_layout_panel(
            this,
            "panel",
            ide::colors::info_text,
            ide::colors::fill_color,
            core::dock::styles::fill);
        _panel->add_child(_header.get());
        _panel->add_child(_button_panel.get());
        _panel->add_child(_top_panel.get());

        _panel->focus(_name_textbox.get());
    }

    void controller::on_update(uint32_t dt) {
    }

    hardware::machine* controller::machine() {
        return _machine;
    }

    void controller::on_draw(core::renderer& surface) {
        _panel->draw(surface);
    }

    void controller::machine(hardware::machine* value) {
        _machine = value;
        update_values();
    }

    void controller::on_resize(const core::rect& bounds) {
        _panel->resize(bounds);
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