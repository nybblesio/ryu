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
            int id,
            const std::string& name) : core::view(context, parent, core::view::types::custom, id, name),
                                       _name_label(context, this, ids::name_label, "name-label"),
                                       _display_label(context, this, ids::display_label, "display-label"),
                                       _name_textbox(context, this, ids::name_textbox, "name-textbox"),
                                       _address_space_label(context, this, ids::address_space_label, "address-space-label"),
                                       _address_space_textbox(context, this, ids::address_space_textbox, "address-space-textbox") {
    }

    editor_view::~editor_view() {
    }

    void editor_view::on_draw() {
    }

    void editor_view::on_resize() {
        auto name_label_width = measure_text(_name_label.value()) + 50;
        _name_label.rect()
                .pos(10, 15)
                .size(name_label_width, font_face()->line_height + 5);
        _name_textbox.rect()
                .pos(name_label_width + 10, 10)
                .size((font_face()->width * 32) + 10, font_face()->line_height + 5);

        _address_space_label.rect()
                .pos(10, 45)
                .size(name_label_width, font_face()->line_height + 5);
        _address_space_textbox.rect()
                .pos(name_label_width + 10, 40)
                .size((font_face()->width * 8) + 10, font_face()->line_height + 5);

        _display_label.rect()
                .pos(10, 75)
                .size(name_label_width, font_face()->line_height + 5);
    }

    void editor_view::initialize(hardware::machine* mach) {
        _machine = mach;
        
        _name_label.font_family(font_family());
        _name_label.fg_color(ide::context::colors::text);
        _name_label.bg_color(ide::context::colors::fill_color);
        _name_label.value("machine name:");
        _name_label.alignment(core::alignment::types::right);

        _name_textbox.font_family(font_family());
        _name_textbox.fg_color(ide::context::colors::text);
        _name_textbox.bg_color(ide::context::colors::fill_color);
        _name_textbox.initialize(1, 32);
        _name_textbox.on_key_down([&](int keycode) {
            return true;
        });
        _name_textbox.on_tab([&]() {
            focus(ids::address_space_textbox);
        });
        _name_textbox.value(_machine->name());
        _name_textbox.enabled(true);

        _address_space_label.font_family(font_family());
        _address_space_label.fg_color(ide::context::colors::text);
        _address_space_label.bg_color(ide::context::colors::fill_color);
        _address_space_label.value("address space:");
        _address_space_label.alignment(core::alignment::types::right);

        _address_space_textbox.font_family(font_family());
        _address_space_textbox.fg_color(ide::context::colors::text);
        _address_space_textbox.bg_color(ide::context::colors::fill_color);
        _address_space_textbox.initialize(1, 8);
        _address_space_textbox.on_key_down([&](int keycode) {
            return true;
        });
        _address_space_textbox.on_tab([&]() {
            focus(ids::name_textbox);
        });
        _address_space_textbox.value(fmt::format("{0:08x}", _machine->address_space()));
        _address_space_textbox.enabled(true);

        _display_label.font_family(font_family());
        _display_label.fg_color(ide::context::colors::text);
        _display_label.bg_color(ide::context::colors::fill_color);
        _display_label.value("display:");
        _display_label.alignment(core::alignment::types::right);

        padding({5, 5, 5, 5});
        on_resize();
    }

    void editor_view::on_focus_changed() {
        if (focused()) {
            focus(ids::name_textbox);
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