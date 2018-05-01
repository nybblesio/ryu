//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#pragma once

#include <SDL_system.h>
#include "rect.h"
#include "core_types.h"
#include "font_family.h"
#include "palette_entry.h"

namespace ryu::core {

    class renderer {
    public:
        renderer() = default;

        void clear();

        void present();

        void draw_text(
                const font_t* font_face,
                int x,
                int y,
                const std::string& value,
                const core::palette_entry& color);

        void draw_text_scaled(
                const font_t* font_face,
                int x,
                int y,
                const std::string& value,
                float sx,
                float sy);

        int measure_text(
                const font_t* font_face,
                const std::string& value);

        void set_font_color(
                const font_t* font_face,
                const core::palette_entry& color);

        void pop_blend_mode();

        void draw_text_aligned(
                const font_t* font_face,
                const std::string& value,
                const core::rect& bounds,
                alignment::horizontal::types halign,
                alignment::vertical::types valign);

        core::rect pop_clip_rect();

        const core::rect& bounds() const;

        void sdl_renderer(SDL_Renderer* value);

        void draw_rect(const core::rect& bounds);

        void fill_rect(const core::rect& bounds);

        void push_blend_mode(SDL_BlendMode mode);

        void set_clip_rect(const core::rect& bounds);

        void push_clip_rect(const core::rect& bounds);

        void draw_line(int x1, int y1, int x2, int y2);

        void fill_polygon(const vertex_list& vertices);

        void set_color(const core::palette_entry& color);

        void draw_selection_rect(core::rect& rect, core::palette_entry color);

    private:
        core::rect _bounds;
        SDL_Renderer* _surface = nullptr;
        std::vector<core::rect> _clip_stack {};
        std::stack<SDL_BlendMode> _mode_stack {};
    };

};

