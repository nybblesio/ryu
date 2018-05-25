//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <core/input_action.h>
#include "button.h"

namespace ryu::core {

    button::button(
        const std::string& name,
        core::view_host* host) : core::view(core::view::types::control, name, host) {
    }

    button::~button() {
    }

    void button::define_actions() {
        auto activate_action = core::input_action::create_no_map(
                "button_activate",
                "Internal",
                "Activate a button view.");
        if (!activate_action->has_bindings())
            activate_action->bind_keys({core::key_space});
    }

    void button::bind_events() {
        action_provider().register_handler(
                core::input_action::find_by_name("button_activate"),
                [this](const event_data_t& data) {
                    if (_on_clicked) {
                        _on_clicked();
                        return true;
                    }
                    return false;
                });
    }

    void button::on_initialize() {
        define_actions();
        bind_events();

        tab_stop(true);
        update_minimum_size();
    }

    void button::update_minimum_size() {
        auto& minimum_size = min_size();
        if (minimum_size.empty()) {
            minimum_size.dimensions(
                125,
                static_cast<uint32_t>(std::max(font_face()->line_height + 20, 50)));
        }
    }

    std::string button::shortcut() const {
        return _shortcut;
    }

    void button::on_font_family_changed() {
        update_minimum_size();
    }

    palette_index button::shortcut_color() const {
        return _shortcut_color;
    }

    void button::on_draw(core::renderer& surface) {
        surface.push_blend_mode(SDL_BLENDMODE_BLEND);

        auto inner_rect = inner_bounds();

        auto pal = *palette();
        auto fg = pal[fg_color()];
        auto bg = pal[bg_color()];
        auto shortcut_color = pal[_shortcut_color];

        if (!enabled()) {
            fg = fg - 45;
            bg = bg - 45;
            shortcut_color = shortcut_color - 45;
        } else if (!focused()) {
            fg = fg - 35;
            bg = bg - 35;
            shortcut_color = shortcut_color - 35;
        }

        surface.set_color(bg);
        surface.fill_rect(inner_rect);

        surface.set_font_color(font_face(), fg);
        core::rect label_bounds {
            inner_rect.left(),
            inner_rect.top() + 6,
            inner_rect.width(),
            inner_rect.height() - 6
        };
        surface.draw_text_aligned(
            font_face(),
            value(),
            label_bounds,
            _halign,
            _valign);

        if (!_shortcut.empty()) {
            surface.set_font_color(font_face(), shortcut_color);
            surface.draw_text_scaled(
                font_face(),
                inner_rect.right() - _shortcut_text_width,
                inner_rect.top() + 2,
                _shortcut,
                .5,
                .5);
        }

        surface.pop_blend_mode();
    }

    void button::shortcut(const std::string& value) {
        _shortcut = value;
        _shortcut_text_width = (font_face()->measure_text(_shortcut) / 2) + 2;
    }

    void button::shortcut_color(palette_index value) {
        _shortcut_color = value;
    }

    alignment::vertical::types button::valign() const {
        return _valign;
    }

    alignment::horizontal::types button::halign() const {
        return _halign;
    }

    void button::valign(alignment::vertical::types value) {
        _valign = value;
    }

    void button::halign(alignment::horizontal::types value) {
        _halign = value;
    }

    void button::on_clicked(const button::on_clicked_callable& callable) {
        _on_clicked = callable;
    }

}