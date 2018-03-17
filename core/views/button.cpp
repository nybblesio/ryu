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

    void button::on_initialize() {
        auto activate_action = core::input_action::create_no_map(
            "button_activate",
            "Internal",
            "Activate a button view.");
        activate_action->register_handler(
            action_sink::types::view,
            [this](const event_data_t& data) {
                return focused() && enabled();
            },
            [this](const event_data_t& data) {
                if (_on_clicked) {
                    _on_clicked();
                    return true;
                }
                return false;
            });
        activate_action->bind_keys({core::key_space});
    }

    void button::width(int value) {
        _width = value;
    }

    void button::height(int value) {
        _height = value;
    }

    std::string button::value() const {
        return _value;
    }

    border::types button::border() const {
        return _border;
    }

    void button::border(border::types value) {
        _border = value;
    }

    void button::value(const std::string& value) {
        _value = value;
    }

    void button::on_draw(core::renderer& surface) {
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

        surface.set_font_color(font_face(), fg);
        surface.draw_text_aligned(font_face(), _value, bounds, _halign, _valign);

        if (_border == border::types::solid) {
            surface.set_color(fg);
            surface.draw_rect(bounds);
        }

        surface.pop_blend_mode();
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