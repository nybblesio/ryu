//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "list_box.h"

namespace ryu::core {

    list_box::list_box(
            const std::string& name,
            core::view_host* host) : core::view(core::view::types::control, name, host) {
    }

    list_box::~list_box() {
    }

    void list_box::define_actions() {
        auto select_action = core::input_action::create_no_map(
            "list_box_select",
            "Internal",
            "Select an item in the list box.");
        if (!select_action->has_bindings())
            select_action->bind_keys({core::key_space});
    }

    void list_box::bind_events() {
        action_provider().register_handler(
            core::input_action::find_by_name("list_box_select"),
            [this](const event_data_t& data) {
                if (_on_clicked) {
                    _on_clicked();
                    return true;
                }
                return false;
            });
    }

    void list_box::on_initialize() {
        tab_stop(true);
        define_actions();
        bind_events();
    }

    border::types list_box::border() const {
        return _border;
    }

    void list_box::border(border::types value) {
        _border = value;
    }

    void list_box::on_draw(core::renderer& surface) {
        surface.push_blend_mode(SDL_BLENDMODE_BLEND);

        auto bounds = client_bounds();

        auto pal = *palette();
        auto fg = pal[fg_color()];
        auto bg = pal[bg_color()];

        if (!enabled()) {
            fg = fg - 45;
            bg = bg - 45;
        } else if (!focused()) {
            fg = fg - 35;
            bg = bg - 35;
        }

        surface.set_color(bg);
        surface.fill_rect(bounds);

        if (_border == border::types::solid) {
            surface.set_color(fg);
            surface.draw_rect(bounds);
        }

        surface.pop_blend_mode();
    }

    void list_box::on_resize(const core::rect& context_bounds) {
        switch (sizing()) {
            case sizing::parent: {
                auto container = parent();
                bounds(container != nullptr ? container->bounds() : context_bounds);
                break;
            }
            default: {
                break;
            }
        }
    }

    void list_box::on_clicked(const list_box::on_clicked_callable& callable) {
        _on_clicked = callable;
    }

}