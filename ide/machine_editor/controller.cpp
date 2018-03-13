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
    }

    void controller::on_deactivate() {
        _machine = nullptr;
    }

    void controller::update_values() {
        int machine_id = 0;
        if (_machine != nullptr) {
            machine_id = _machine->id();
            _name_textbox->value(_machine->name());
            auto disp = _machine->display();
            if (disp != nullptr)
                _display_pick_list->value(disp->name());
            _address_space_textbox->value(fmt::format("{0:08x}", _machine->address_space()));
        }
        _header->value(fmt::format("machine editor | id: {}", machine_id));
        _header->resize(context()->bounds());
    }

    void controller::on_initialize() {
        bind_events();

        auto address_space_label_width = context()->font_face()->measure_text("address space: $");

        _header = core::view_factory::create_label(
            this,
            "header-label",
            ide::colors::info_text,
            ide::colors::fill_color,
            "",
            core::dock::styles::top,
            {_metrics.left_padding, _metrics.right_padding, 5, 15});

        _footer = core::view_factory::create_label(
            this,
            "footer-label",
            ide::colors::info_text,
            ide::colors::fill_color,
            "F1=Map | F2=Add | DEL=Remove | ESC=Close",
            core::dock::styles::bottom,
            {_metrics.left_padding, _metrics.right_padding, 5, 5});

        _name_label = core::view_factory::create_label(
            this,
            "name-label",
            ide::colors::text,
            ide::colors::fill_color,
            "name:",
            core::dock::styles::left);
        _name_label->sizing(core::view::sizing::types::fixed);
        _name_label->halign(core::alignment::horizontal::right);
        _name_label->bounds().size(
                address_space_label_width,
                context()->font_face()->line_height);

        _name_textbox = core::view_factory::create_textbox(
            this,
            "name-textbox",
            ide::colors::text,
            ide::colors::fill_color,
            "",
            core::dock::styles::left,
            {15, 0, 3, 0});
        _name_textbox->width(32);
        _name_textbox->length(32);
        _name_textbox->enabled(true);
        _name_textbox->on_tab([&]() -> const core::view* { return _address_space_textbox.get(); });
        _name_textbox->on_key_down([&](int keycode) {
            return true;
        });

        _address_space_label = core::view_factory::create_label(
            this,
            "address-space-label",
            ide::colors::text,
            ide::colors::fill_color,
            "address space: $",
            core::dock::styles::left);
        _address_space_label->sizing(core::view::sizing::types::fixed);
        _address_space_label->halign(core::alignment::horizontal::right);
        _address_space_label->bounds().size(
                address_space_label_width,
                context()->font_face()->line_height);

        _address_space_textbox = core::view_factory::create_textbox(
            this,
            "address-space-textbox",
            ide::colors::text,
            ide::colors::fill_color,
            "",
            core::dock::styles::left,
            {15, 0, 3, 0});
        _address_space_textbox->width(8);
        _address_space_textbox->length(8);
        _address_space_textbox->enabled(true);
        _address_space_textbox->on_tab([&]() -> const core::view* { return _display_pick_list.get(); });
        _address_space_textbox->on_key_down([](int keycode) {
            return isxdigit(keycode);
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
        _display_label->bounds().size(
                address_space_label_width,
                context()->font_face()->line_height);

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
        _display_pick_list->on_tab([&]() -> const core::view* { return _map_button.get(); });
        _display_pick_list->bounds().size(
                context()->font_face()->width * _display_pick_list->length(),
                context()->font_face()->line_height);

        _map_button = core::view_factory::create_button(
            this,
            "map-button",
            ide::colors::light_grey,
            ide::colors::light_blue,
            "Map",
            core::dock::styles::left,
            {5, 5, 5, 5});
        _map_button->on_tab([&]() -> const core::view* { return _add_button.get(); });

        _add_button = core::view_factory::create_button(
            this,
            "add-button",
            ide::colors::light_grey,
            ide::colors::light_blue,
            "Add",
            core::dock::styles::left,
            {5, 5, 5, 5});
        _add_button->on_tab([&]() -> const core::view* { return _delete_button.get(); });

        _delete_button = core::view_factory::create_button(
            this,
            "delete-button",
            ide::colors::light_grey,
            ide::colors::light_blue,
            "Delete",
            core::dock::styles::left,
            {5, 5, 5, 5});
        _delete_button->on_tab([&]() -> const core::view* { return _notebook.get(); });

        _row1_panel = core::view_factory::create_dock_layout_panel(
            this,
            "row1-panel",
            ide::colors::info_text,
            ide::colors::transparent,
            core::dock::styles::top,
            {_metrics.left_padding, _metrics.right_padding, 5, 5});
        _row1_panel->bounds().height(context()->font_face()->line_height * 2);
        _row1_panel->add_child(_name_label.get());
        _row1_panel->add_child(_name_textbox.get());

        _row2_panel = core::view_factory::create_dock_layout_panel(
            this,
            "row2-panel",
            ide::colors::info_text,
            ide::colors::transparent,
            core::dock::styles::top,
            {_metrics.left_padding, _metrics.right_padding, 5, 5});
        _row2_panel->bounds().height(context()->font_face()->line_height * 2);
        _row2_panel->add_child(_address_space_label.get());
        _row2_panel->add_child(_address_space_textbox.get());

        _row3_panel = core::view_factory::create_dock_layout_panel(
            this,
            "row3-panel",
            ide::colors::info_text,
            ide::colors::transparent,
            core::dock::styles::top,
            {_metrics.left_padding, _metrics.right_padding, 5, 5});
        _row3_panel->bounds().height(context()->font_face()->line_height * 2);
        _row3_panel->add_child(_display_label.get());
        _row3_panel->add_child(_display_pick_list.get());

        _button_panel = core::view_factory::create_dock_layout_panel(
            this,
            "button-panel",
            ide::colors::info_text,
            ide::colors::transparent,
            core::dock::styles::bottom,
            {_metrics.left_padding, _metrics.right_padding, 5, 5});
        _button_panel->bounds().height(context()->font_face()->line_height * 3);
        _button_panel->add_child(_map_button.get());
        _button_panel->add_child(_add_button.get());
        _button_panel->add_child(_delete_button.get());

        _general_panel = core::view_factory::create_dock_layout_panel(
            this,
            "general-panel",
            ide::colors::info_text,
            ide::colors::fill_color,
            core::dock::styles::fill);
        _general_panel->add_child(_row1_panel.get());
        _general_panel->add_child(_row2_panel.get());
        _general_panel->add_child(_row3_panel.get());

        _notebook = core::view_factory::create_notebook(
            this,
            "delete-button",
            ide::colors::info_text,
            ide::colors::fill_color,
            core::dock::styles::fill,
            {_metrics.left_padding, _metrics.right_padding, 5, 15});
        _notebook->on_tab([&]() -> const core::view* { return _name_textbox.get(); });
        _notebook->add_tab("General", _general_panel.get());
        _notebook->add_tab("Components", nullptr);
        _notebook->add_tab("Settings", nullptr);

        _panel = core::view_factory::create_dock_layout_panel(
            this,
            "panel",
            ide::colors::info_text,
            ide::colors::fill_color,
            core::dock::styles::fill);
        _panel->add_child(_header.get());
        _panel->add_child(_footer.get());
        _panel->add_child(_button_panel.get());
        _panel->add_child(_notebook.get());
        _panel->focus(_notebook.get());
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