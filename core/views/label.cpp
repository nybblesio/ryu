//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <logger_factory.h>
#include "label.h"

namespace ryu::core {

    static logger* s_log = logger_factory::instance()->create(
            "label",
            logger::level::info);

    label::label(
            const std::string& name,
            core::view_host* host) : core::view(types::control, name, host) {
    }

    label::~label() {
    }

    void label::update_content_bounds() {
        auto face = font_face();
        if (face != nullptr) {
            auto width = face->measure_text(_value);
            bounds().size(width, face->line_height);
        }
    }

    std::string label::value() const {
        return _value;
    }

    border::types label::border() const {
        return _border;
    }

    void label::border(border::types value) {
        _border = value;
    }

    void label::value(const std::string& value) {
        if (_value != value) {
            _value = value;
            update_content_bounds();
            requires_layout();
        }
    }

    void label::on_draw(core::renderer& surface) {
        auto bounds = client_bounds();

//        if (name() == "name-label" || name() == "display-label") {
//            s_log->info(fmt::format(
//                "name: {} = [left: {}, right: {}, width: {}], [top: {}, bottom: {}, height: {}]",
//                name(),
//                bounds.left(),
//                bounds.right(),
//                bounds.width(),
//                bounds.top(),
//                bounds.bottom(),
//                bounds.height()));
//        }

        auto current_palette = palette();
        if (current_palette == nullptr)
            return;

        auto& fg = current_palette->get(fg_color());
        auto& bg = current_palette->get(bg_color());

        surface.set_color(bg);
        surface.fill_rect(bounds);
        surface.set_font_color(font_face(), fg);
        surface.draw_text_aligned(
            font_face(),
            _value,
            bounds,
            _halign,
            _valign);
        if (_border != border::types::none) {
            surface.set_color(fg);
            surface.draw_rect(bounds);
        }
    }

    alignment::vertical::types label::valign() const {
        return _valign;
    }

    alignment::horizontal::types label::halign() const {
        return _halign;
    }

    void label::valign(alignment::vertical::types value) {
        _valign = value;
    }

    void label::halign(alignment::horizontal::types value) {
        _halign = value;
    }

    void label::on_resize(const core::rect& context_bounds) {
        switch (sizing()) {
            case sizing::content: {
                update_content_bounds();
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

}