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

    int button::width() const {
        return _width;
    }

    int button::height() const {
        return _height;
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
        tab_stop(true);
        define_actions();
        bind_events();
    }

    void button::width(int value) {
        _width = value;
    }

    void button::height(int value) {
        _height = value;
    }

    std::string button::shortcut() const {
        return _shortcut;
    }

    border::types button::border() const {
        return _border;
    }

    uint8_t button::shortcut_color() const {
        return _shortcut_color;
    }

    void button::border(border::types value) {
        _border = value;
    }

    void button::on_draw(core::renderer& surface) {
        surface.push_blend_mode(SDL_BLENDMODE_BLEND);

        auto bounds = client_bounds();

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
        surface.fill_rect(bounds);

        surface.set_font_color(font_face(), fg);
        core::rect label_bounds {
            bounds.left(),
            bounds.top() + 6,
            bounds.width(),
            bounds.height() - 6
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
                bounds.right() - _shortcut_text_width,
                bounds.top() + 2,
                _shortcut,
                .5,
                .5);
        }

        if (_border == border::types::solid) {
            surface.set_color(fg);
            surface.draw_rect(bounds);
        }

        surface.pop_blend_mode();
    }

    void button::shortcut_color(uint8_t value) {
        _shortcut_color = value;
    }

    void button::shortcut(const std::string& value) {
        _shortcut = value;
        _shortcut_text_width = (font_face()->measure_text(_shortcut) / 2) + 2;
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

    void button::on_resize(const core::rect& context_bounds) {
        switch (sizing()) {
            case sizing::content: {
                bounds().size(_width, _height);
                break;
            }
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

    void button::on_clicked(const button::on_clicked_callable& callable) {
        _on_clicked = callable;
    }

}