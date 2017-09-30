//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "label.h"

namespace ryu::core {

    label::label(
            core::context* context,
            core::view* parent,
            const std::string& name) : core::view(context, parent, types::label, name) {
    }

    std::string label::value() const {
        return _value;
    }

    alignment::types label::alignment() const {
        return _alignment;
    }

    void label::value(const std::string& value) {
        _value = value;
    }

    void label::alignment(alignment::types value) {
        _alignment = value;
    }

    void label::on_draw() {
        auto bounds = client_rect();

        auto& fg = (*context()->palette())[fg_color()];
        auto& bg = (*context()->palette())[bg_color()];

        set_color(bg);
        fill_rect(bounds);

        set_font_color(fg);
        draw_text_aligned(_value, bounds, _alignment);
    }

}