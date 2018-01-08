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
#include "controller.h"

namespace ryu::ide::machine_editor {

    controller::controller(const std::string& name) : core::state(name),
                                                      _header("header-label"),
                                                      _footer("footer-label"),
                                                      _name_label("name-label"),
                                                      _notebook("notebook"),
                                                      _map_button("map-button"),
                                                      _add_button("add-button"),
                                                      _display_label("display-label"),
                                                      _delete_button("delete-button"),
                                                      _name_textbox("name-textbox"),
                                                      _panel("panel"),
                                                      _address_space_label("address-space-label"),
                                                      _display_pick_list("display-pick-list"),
                                                      _row1_panel("row1"),
                                                      _row2_panel("row2"),
                                                      _row3_panel("roe3"),
                                                      _address_space_textbox("address-space-textbox"),
                                                      _button_panel("buttons"),
                                                      _general_panel("layout-panel") {
    }

    void controller::on_deactivate() {
        _machine = nullptr;
    }

    void controller::update_values() {
        int machine_id = 0;
        if (_machine != nullptr) {
            machine_id = _machine->id();
            _name_textbox.value(_machine->name());
            auto disp = _machine->display();
            if (disp != nullptr)
                _display_pick_list.value(disp->name());
            _address_space_textbox.value(fmt::format("{0:08x}", _machine->address_space()));
        }
        std::string project_name = "(none)";
//        if (_project != nullptr) {
//            project_name = _project->name();
//        }
        _header.value(fmt::format("machine editor | project: {0} | id: {1}", project_name, machine_id));
        _header.resize(context()->bounds());
    }

    void controller::on_initialize() {
        auto family = context()->engine()->find_font_family("hack");
        auto face = family->find_style(core::font::styles::normal);
        auto address_space_label_width = face->measure_text("address space: $");

        _header.font_family(family);
        _header.palette(context()->palette());
        _header.dock(core::dock::styles::top);
        _header.fg_color(ide::colors::info_text);
        _header.bg_color(ide::colors::fill_color);
        _header.margin({_metrics.left_padding, _metrics.right_padding, 5, 15});

        _footer.font_family(family);
        _footer.palette(context()->palette());
        _footer.dock(core::dock::styles::bottom);
        _footer.fg_color(ide::colors::info_text);
        _footer.bg_color(ide::colors::fill_color);
        _footer.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
        _footer.value("F1=Map | F2=Add | DEL=Remove | ESC=Close");

        _name_label.value("name:");
        _name_label.font_family(family);
        _name_label.margin({0, 0, 0, 0});
        _name_label.palette(context()->palette());
        _name_label.dock(core::dock::styles::left);
        _name_label.fg_color(ide::colors::text);
        _name_label.bg_color(ide::colors::fill_color);
        _name_label.sizing(core::view::sizing::types::fixed);
        _name_label.halign(core::alignment::horizontal::right);
        _name_label.bounds().size(address_space_label_width, face->line_height);

        _name_textbox.width(32);
        _name_textbox.length(32);
        _name_textbox.enabled(true);
        _name_textbox.on_tab([&]() -> const core::view* { return &_address_space_textbox; });
        _name_textbox.font_family(family);
        _name_textbox.margin({15, 0, 3, 0});
        _name_textbox.palette(context()->palette());
        _name_textbox.dock(core::dock::styles::left);
        _name_textbox.fg_color(ide::colors::text);
        _name_textbox.bg_color(ide::colors::fill_color);
        _name_textbox.on_key_down([&](int keycode) {
            return true;
        });

        _address_space_label.font_family(family);
        _address_space_label.margin({0, 0, 0, 0});
        _address_space_label.value("address space: $");
        _address_space_label.palette(context()->palette());
        _address_space_label.dock(core::dock::styles::left);
        _address_space_label.fg_color(ide::colors::text);
        _address_space_label.bg_color(ide::colors::fill_color);
        _address_space_label.sizing(core::view::sizing::types::fixed);
        _address_space_label.halign(core::alignment::horizontal::right);
        _address_space_label.bounds().size(address_space_label_width, face->line_height);

        _address_space_textbox.width(8);
        _address_space_textbox.length(8);
        _address_space_textbox.enabled(true);
        _address_space_textbox.on_tab([&]() -> const core::view* { return &_display_pick_list; });
        _address_space_textbox.font_family(family);
        _address_space_textbox.margin({15, 0, 3, 0});
        _address_space_textbox.palette(context()->palette());
        _address_space_textbox.dock(core::dock::styles::left);
        _address_space_textbox.fg_color(ide::colors::text);
        _address_space_textbox.bg_color(ide::colors::fill_color);
        _address_space_textbox.on_key_down([&](int keycode) {
            return isxdigit(keycode);
        });

        _display_label.value("display:");
        _display_label.font_family(family);
        _display_label.margin({0, 0, 0, 0});
        _display_label.palette(context()->palette());
        _display_label.dock(core::dock::styles::left);
        _display_label.fg_color(ide::colors::text);
        _display_label.bg_color(ide::colors::fill_color);
        _display_label.sizing(core::view::sizing::types::fixed);
        _display_label.halign(core::alignment::horizontal::right);
        _display_label.bounds().size(address_space_label_width, face->line_height);

        auto& displays = _display_pick_list.options();
        for (auto& display : hardware::display::catalog()) {
            displays.push_back(display.name());
        }
        _display_pick_list.font_family(family);
        _display_pick_list.margin({15, 0, 4, 0});
        _display_pick_list.palette(context()->palette());
        _display_pick_list.dock(core::dock::styles::left);
        _display_pick_list.fg_color(ide::colors::text);
        _display_pick_list.bg_color(ide::colors::fill_color);
        _display_pick_list.on_tab([&]() -> const core::view* { return &_map_button; });
        _display_pick_list.bounds().size(
                face->width * _display_pick_list.length(),
                face->line_height);

        _map_button.value("Map");
        _map_button.font_family(family);
        _map_button.margin({5, 5, 5, 5});
        _map_button.palette(context()->palette());
        _map_button.dock(core::dock::styles::left);
        _map_button.fg_color(ide::colors::light_grey);
        _map_button.bg_color(ide::colors::light_blue);
        _map_button.on_tab([&]() -> const core::view* { return &_add_button; });

        _add_button.value("Add");
        _add_button.font_family(family);
        _add_button.margin({5, 5, 5, 5});
        _add_button.palette(context()->palette());
        _add_button.dock(core::dock::styles::left);
        _add_button.fg_color(ide::colors::light_grey);
        _add_button.bg_color(ide::colors::light_blue);
        _add_button.on_tab([&]() -> const core::view* { return &_delete_button; });

        _delete_button.value("Delete");
        _delete_button.font_family(family);
        _delete_button.margin({5, 5, 5, 5});
        _delete_button.palette(context()->palette());
        _delete_button.dock(core::dock::styles::left);
        _delete_button.fg_color(ide::colors::light_grey);
        _delete_button.bg_color(ide::colors::light_blue);
        _delete_button.on_tab([&]() -> const core::view* { return &_notebook; });

        _row1_panel.palette(context()->palette());
        _row1_panel.dock(core::dock::styles::top);
        _row1_panel.bg_color(ide::colors::transparent);
        _row1_panel.bounds().height(face->line_height * 2);
        _row1_panel.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
        _row1_panel.add_child(&_name_label);
        _row1_panel.add_child(&_name_textbox);

        _row2_panel.palette(context()->palette());
        _row2_panel.dock(core::dock::styles::top);
        _row2_panel.bg_color(ide::colors::transparent);
        _row2_panel.bounds().height(face->line_height * 2);
        _row2_panel.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
        _row2_panel.add_child(&_address_space_label);
        _row2_panel.add_child(&_address_space_textbox);

        _row3_panel.palette(context()->palette());
        _row3_panel.dock(core::dock::styles::top);
        _row3_panel.bg_color(ide::colors::transparent);
        _row3_panel.bounds().height(face->line_height * 2);
        _row3_panel.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
        _row3_panel.add_child(&_display_label);
        _row3_panel.add_child(&_display_pick_list);

        _button_panel.palette(context()->palette());
        _button_panel.dock(core::dock::styles::bottom);
        _button_panel.bg_color(ide::colors::transparent);
        _button_panel.bounds().height(face->line_height * 3);
        _button_panel.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
        _button_panel.add_child(&_map_button);
        _button_panel.add_child(&_add_button);
        _button_panel.add_child(&_delete_button);

        _general_panel.font_family(family);
        _general_panel.palette(context()->palette());
        _general_panel.dock(core::dock::styles::fill);
        _general_panel.fg_color(ide::colors::info_text);
        _general_panel.bg_color(ide::colors::fill_color);
        _general_panel.add_child(&_row1_panel);
        _general_panel.add_child(&_row2_panel);
        _general_panel.add_child(&_row3_panel);

        _notebook.on_tab([&]() -> const core::view* { return &_name_textbox; });
        _notebook.font_family(family);
        _notebook.palette(context()->palette());
        _notebook.dock(core::dock::styles::fill);
        _notebook.fg_color(ide::colors::info_text);
        _notebook.bg_color(ide::colors::fill_color);
        _notebook.margin({_metrics.left_padding, _metrics.right_padding, 5, 15});
        _notebook.add_tab("General", &_general_panel);
        _notebook.add_tab("Components", nullptr);
        _notebook.add_tab("Settings", nullptr);

        _panel.font_family(family);
        _panel.palette(context()->palette());
        _panel.dock(core::dock::styles::fill);
        _panel.fg_color(ide::colors::info_text);
        _panel.bg_color(ide::colors::fill_color);
        _panel.add_child(&_header);
        _panel.add_child(&_footer);
        _panel.add_child(&_button_panel);
        _panel.add_child(&_notebook);
        _panel.focus(&_notebook);

//        auto base = rttr::type::get<ryu::hardware::integrated_circuit>();
//        auto component_types = base.get_derived_classes();
//        for(auto& t : component_types) {
//            std::cout << t.get_name() << "\n";
//            std::cout << "properties:\n";
//            for (auto& p : t.get_properties())
//                std::cout << "\tname: " << p.get_name() << "\n";
//        }
    }

    void controller::on_update(uint32_t dt) {
    }

    hardware::machine* controller::machine() {
        return _machine;
    }

    void controller::on_draw(core::renderer& surface) {
        _panel.draw(surface);
    }

    void controller::machine(hardware::machine* value) {
        _machine = value;
        update_values();
    }

    void controller::on_resize(const core::rect& bounds) {
        _panel.resize(bounds);
    }

    bool controller::on_process_event(const SDL_Event* e) {
        if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_ESCAPE: {
                    end_state();
                    return true;
                }
            }
        }
        return _panel.process_event(e);
    }

    void controller::on_activate(const core::parameter_dict& params) {
        auto it = params.find("name");
        if (it != params.end()) {
            auto mach = hardware::registry::instance()->find_machine(it->second);
            if (mach == nullptr) {
                mach = hardware::registry::instance()->new_machine();
                mach->name(it->second);
            }
            machine(mach);
        }
    }

}