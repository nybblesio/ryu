//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "button.h"

namespace ryu::core {

    button::button(
        const std::string& name,
        core::view_container* container) : core::view(core::view::types::control, name, container) {
    }

    int button::width() const {
        return _width;
    }

    int button::height() const {
        return _height;
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

//    bool button::on_process_event(const SDL_Event* e) {
//        if (e->type == SDL_KEYDOWN) {
//            switch (e->key.keysym.sym) {
//                case SDLK_SPACE: {
//                    if (_on_clicked) {
//                        _on_clicked();
//                    }
//                    return true;
//                }
//            }
//        }
//        return false;
//    }

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