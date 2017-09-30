//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <fmt/format.h>
#include <ide/context.h>
#include "editor_view.h"

namespace ryu::ide::machine_editor {

    editor_view::editor_view(
            core::context* context,
            core::view* parent,
            const std::string& name) : core::view(context, parent, core::view::types::container, name),
                                       _header(context, this, "header-label"),
                                       _footer(context, this, "footer-label"),
                                       _row1_panel(context, this, "row1"),
                                       _row2_panel(context, this, "row2"),
                                       _name_label(context, &_row1_panel, "name-label"),
                                       _name_textbox(context, &_row1_panel, "name-textbox"),
                                       _address_space_label(context, &_row1_panel, "address-space-label"),
                                       _address_space_textbox(context, &_row1_panel, "address-space-textbox"),
                                       _display_label(context, &_row2_panel, "display-label") {
    }

    editor_view::~editor_view() {
    }

    void editor_view::on_draw() {
    }

    void editor_view::initialize(hardware::machine* mach) {
        _machine = mach;

        _header.dock(dock::styles::top);
        _header.font_family(font_family());
        _header.fg_color(ide::context::colors::info_text);
        _header.bg_color(ide::context::colors::fill_color);
        _header.bounds().height(font_face()->line_height);
        _header.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});

        std::string project_name = "(none)";
        auto project = dynamic_cast<ide::context*>(context())->project();
        if (project != nullptr)
            project_name = project->name();
        _header.value(fmt::format("machine editor | project: {0} | id: {1}", project_name, _machine->id()));

        _footer.dock(dock::styles::bottom);
        _footer.font_family(font_family());
        _footer.fg_color(ide::context::colors::info_text);
        _footer.bg_color(ide::context::colors::fill_color);
        _footer.bounds().height(font_face()->line_height);
        _footer.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
        _footer.value("F1=Map | F2=Add | DEL=Remove | ESC=Close");

        _name_label.value("name:");
        _name_label.margin({0, 0, 0, 0});
        _name_label.dock(dock::styles::left);
        _name_label.font_family(font_family());
        _name_label.fg_color(ide::context::colors::text);
        _name_label.bg_color(ide::context::colors::fill_color);
        _name_label.halign(core::alignment::horizontal::right);
        _name_label.bounds().size(measure_text("display:"), font_face()->line_height);

        _name_textbox.enabled(true);
        _name_textbox.margin({5, 0, 5, 0});
        _name_textbox.dock(dock::styles::left);
        _name_textbox.font_family(font_family());
        _name_textbox.fg_color(ide::context::colors::text);
        _name_textbox.bg_color(ide::context::colors::fill_color);
        _name_textbox.initialize(1, 32);
        _name_textbox.on_key_down([&](int keycode) {
            return true;
        });
        _name_textbox.on_tab([&]() {
            focus(_address_space_textbox.id());
        });
        _name_textbox.value(_machine->name());
        _name_textbox.bounds().size(font_face()->width * _name_textbox.length(), font_face()->line_height);

        _address_space_label.margin({10, 0, 0, 0});
        _address_space_label.dock(dock::styles::left);
        _address_space_label.value("address space: $");
        _address_space_label.font_family(font_family());
        _address_space_label.fg_color(ide::context::colors::text);
        _address_space_label.bg_color(ide::context::colors::fill_color);
        _address_space_label.halign(core::alignment::horizontal::left);
        _address_space_label.bounds().size(measure_text(_address_space_label.value()), font_face()->line_height);

        _address_space_textbox.enabled(true);
        _address_space_textbox.margin({5, 0, 5, 0});
        _address_space_textbox.dock(dock::styles::left);
        _address_space_textbox.font_family(font_family());
        _address_space_textbox.fg_color(ide::context::colors::text);
        _address_space_textbox.bg_color(ide::context::colors::fill_color);
        _address_space_textbox.initialize(1, 8);
        _address_space_textbox.on_key_down([&](int keycode) {
            return true;
        });
        _address_space_textbox.on_tab([&]() {
            focus(_name_textbox.id());
        });
        _address_space_textbox.value(fmt::format("{0:08x}", _machine->address_space()));
        _address_space_textbox.bounds().size(
                font_face()->width * _address_space_textbox.length(),
                font_face()->line_height);

        _display_label.value("display:");
        _display_label.margin({0, 0, 0, 0});
        _display_label.dock(dock::styles::left);
        _display_label.font_family(font_family());
        _display_label.fg_color(ide::context::colors::text);
        _display_label.bg_color(ide::context::colors::fill_color);
        _display_label.halign(core::alignment::horizontal::left);
        _display_label.bounds().size(measure_text(_display_label.value()), font_face()->line_height);

        _row1_panel.dock(dock::styles::top);
        _row1_panel.bounds().height(font_face()->line_height * 2);
        _row1_panel.bg_color(ide::context::colors::transparent);
        _row1_panel.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});

        _row2_panel.dock(dock::styles::top);
        _row2_panel.bounds().height(font_face()->line_height * 2);
        _row2_panel.bg_color(ide::context::colors::transparent);
        _row2_panel.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});

        dock(dock::styles::fill);
        margin({0, 0, 0, 0});
        resize();
    }

    void editor_view::on_focus_changed() {
        if (focused()) {
            focus(_name_textbox.id());
        }
    }

    bool editor_view::on_process_event(const SDL_Event* e) {
//        auto ctrl_pressed = (SDL_GetModState() & KMOD_CTRL) != 0;
//        auto shift_pressed = (SDL_GetModState() & KMOD_SHIFT) != 0;

        if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_TAB: {
                    return true;
                }
            }
        }

        return false;
    }

}