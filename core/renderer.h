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
        explicit renderer(SDL_Renderer* surface);

        SDL_Renderer* ptr() {
            return _surface;
        }

        void clear();

        void present();

        void draw_text(
                const font_t* font_face,
                int x,
                int y,
                const std::string& value,
                const core::palette_entry& color);

        int measure_text(
                const font_t* font_face,
                const std::string& value);

        void set_font_color(
                const font_t* font_face,
                const core::palette_entry& color);

        void pop_clip_rect();

        void pop_blend_mode();

        void draw_text_aligned(
                const font_t* font_face,
                const std::string& value,
                const core::rect& bounds,
                alignment::horizontal::types halign,
                alignment::vertical::types valign);

        void draw_rect(const core::rect& bounds);

        void fill_rect(const core::rect& bounds);

        void push_blend_mode(SDL_BlendMode mode);

        void set_clip_rect(const core::rect& bounds);

        void push_clip_rect(const core::rect& bounds);

        void draw_line(int x1, int y1, int x2, int y2);

        void set_color(const core::palette_entry& color);

    private:
        SDL_Renderer* _surface = nullptr;
        std::stack<SDL_Rect> _clip_stack {};
        std::stack<SDL_BlendMode> _mode_stack {};
    };

};

