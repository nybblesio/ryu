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
            const std::string& name) : core::view(context, parent, core::view::types::custom, name),
                                       _header(context, this, "header-label"),
                                       _footer(context, this, "footer-label"),
                                       _name_label(context, this, "name-label"),
                                       _display_label(context, this, "display-label"),
                                       _name_textbox(context, this, "name-textbox"),
                                       _address_space_label(context, this, "address-space-label"),
                                       _address_space_textbox(context, this, "address-space-textbox") {
    }

    editor_view::~editor_view() {
    }

    void editor_view::on_draw() {
    }

    void editor_view::on_resize() {
        auto bounds = context()->bounds();

        auto height = font_face()->line_height + 6;
        auto name_label_width = measure_text(_name_label.value());
        auto display_label_width = measure_text(_display_label.value());
        auto address_space_label_width = measure_text(_address_space_label.value());
        auto name_textbox_width = (font_face()->width * 32) + 10;
        auto address_textbox_width = (font_face()->width * 8) + 10;

        auto y = 35;
        auto name_offset = display_label_width;
        _name_label.rect()
                .pos(10, y)
                .size(name_offset, height);
        _name_textbox.rect()
                .pos(name_offset + 5, y - 5)
                .size(name_textbox_width, height);

        auto address_space_offset = name_offset + name_textbox_width + 20;
        _address_space_label.rect()
                .pos(address_space_offset, y)
                .size(address_space_label_width, height);
        _address_space_textbox.rect()
                .pos(address_space_offset + address_space_label_width, y - 5)
                .size(address_textbox_width, height);


        y += 25;
        _display_label.rect()
                .pos(10, y)
                .size(display_label_width, height);

        _header.rect()
                .pos(0, 0)
                .size(bounds.width(), _metrics.header_padding);

        _footer.rect()
                .pos(0, bounds.height() - (_metrics.footer_padding + 10))
                .size(bounds.width(), _metrics.footer_padding);
    }

    void editor_view::initialize(hardware::machine* mach) {
        _machine = mach;

        _metrics.footer_padding = font_face()->line_height;
        _metrics.header_padding = font_face()->line_height * 2;

        _header.font_family(font_family());
        _header.bg_color(ide::context::colors::fill_color);
        _header.fg_color(ide::context::colors::info_text);
        _header.padding({_metrics.left_padding, _metrics.right_padding, 5, 0});
        std::string project_name = "(none)";
        auto project = dynamic_cast<ide::context*>(context())->project();
        if (project != nullptr)
            project_name = project->name();
        _header.value(fmt::format("machine editor | project: {0} | id: {1}", project_name, _machine->id()));

        _footer.font_family(font_family());
        _footer.bg_color(ide::context::colors::fill_color);
        _footer.fg_color(ide::context::colors::info_text);
        _footer.padding({_metrics.left_padding, _metrics.right_padding, 0, 0});
        _footer.value("F1=Memory Map | F2=Add Component | DEL=Remove Component | ESC=Close");

        _name_label.font_family(font_family());
        _name_label.fg_color(ide::context::colors::text);
        _name_label.bg_color(ide::context::colors::fill_color);
        _name_label.value("name:");
        _name_label.alignment(core::alignment::types::right);

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
        _name_textbox.enabled(true);

        _address_space_label.font_family(font_family());
        _address_space_label.fg_color(ide::context::colors::text);
        _address_space_label.bg_color(ide::context::colors::fill_color);
        _address_space_label.value("address space: $");
        _address_space_label.alignment(core::alignment::types::left);

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
        _address_space_textbox.enabled(true);

        _display_label.font_family(font_family());
        _display_label.fg_color(ide::context::colors::text);
        _display_label.bg_color(ide::context::colors::fill_color);
        _display_label.value("display:");
        _display_label.alignment(core::alignment::types::left);

        padding({5, 5, 5, 5});
        on_resize();
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