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
            int id,
            const std::string& name) : core::view(context, parent, types::label, id, name) {
    }

    std::string label::value() const {
        return _value;
    }

    void label::value(const std::string& value) {
        _value = value;
    }

    void label::on_draw(SDL_Renderer* renderer) {
        auto bounds = client_rect();

        auto& fg = (*context()->palette())[fg_color()];
        auto& bg = (*context()->palette())[bg_color()];

        FC_AlignEnum align = FC_AlignEnum::FC_ALIGN_LEFT;
        switch (_alignment) {
            case alignment::none:
            case alignment::left:
                align = FC_AlignEnum::FC_ALIGN_LEFT;
                break;
            case alignment::right:
                align = FC_AlignEnum::FC_ALIGN_RIGHT;
                break;
            case alignment::center:
                align = FC_AlignEnum::FC_ALIGN_CENTER;
                break;
        }

        auto rect = bounds.to_sdl_rect();

        SDL_SetRenderDrawColor(renderer, bg.red(), bg.green(), bg.blue(), bg.alpha());
        SDL_RenderFillRect(renderer, &rect);

        auto draw_font = font();
        FC_SetDefaultColor(draw_font->glyph, fg.to_sdl_color());
        FC_DrawBoxAlign(draw_font->glyph, renderer, rect, align, _value.c_str());
    }

    label::alignment::types label::alignment() const {
        return _alignment;
    }

    void label::alignment(label::alignment::types value) {
        _alignment = value;
    }

}