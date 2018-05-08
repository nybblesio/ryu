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

    std::string label::value() const {
        return view::value();
    }

    void label::update_content_bounds() {
        auto face = font_face();
        if (face != nullptr) {
            auto width = face->measure_text(value());
            bounds().size(width, face->line_height);
        }
    }

    void label::value(const std::string& text) {
        if (view::value() != text) {
            view::value(text);
            update_content_bounds();
        }
    }

    void label::on_draw(ryu::core::renderer& surface) {
        auto bounds = inner_bounds();

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
            value(),
            bounds,
            _halign,
            _valign);
    }

    core::alignment::vertical::types label::valign() const {
        return _valign;
    }

    core::alignment::horizontal::types label::halign() const {
        return _halign;
    }

    void label::valign(core::alignment::vertical::types value) {
        _valign = value;
    }

    void label::halign(core::alignment::horizontal::types value) {
        _halign = value;
    }

}