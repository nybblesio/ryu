//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <ext/SDL_FontCache/SDL_FontCache.h>
#include "renderer.h"

namespace ryu::core {

    void renderer::clear() {
        SDL_RenderClear(_surface);
    }

    void renderer::draw_text(
            const font_t* font_face,
            int x,
            int y,
            const std::string& value,
            const core::palette_entry& color) {
        FC_DrawColor(
                font_face->glyph,
                _surface,
                x,
                y,
                color.to_sdl_color(),
                value.c_str());
    }

    void renderer::draw_text_scaled(
            const font_t* font_face,
            int x,
            int y,
            const std::string& value,
            float sx,
            float sy) {
        FC_DrawScale(
            font_face->glyph,
            _surface,
            x,
            y,
            FC_MakeScale(sx, sy),
            value.c_str());
    }

    void renderer::present() {
        SDL_RenderPresent(_surface);
    }

    void renderer::set_font_color(
            const font_t* font_face,
            const core::palette_entry& color) {
        FC_SetDefaultColor(font_face->glyph, color.to_sdl_color());
    }

    void renderer::pop_blend_mode() {
        if (_mode_stack.empty())
            return;
        auto mode = _mode_stack.top();
        _mode_stack.pop();
        SDL_SetRenderDrawBlendMode(_surface, mode);
    }

    void renderer::draw_text_aligned(
            const font_t* font_face,
            const std::string& value,
            const core::rect& bounds,
            alignment::horizontal::types halign,
            alignment::vertical::types valign) {
        auto yoffset = 0;
        switch (valign) {
            case alignment::vertical::bottom:
                yoffset = bounds.height() - font_face->line_height;
                break;
            case alignment::vertical::middle:
                yoffset = (bounds.height() - font_face->line_height) / 2;
                break;
            default:
                break;
        }
        auto box = SDL_Rect{bounds.left(), bounds.top() + yoffset, bounds.width(), bounds.height()};
        FC_DrawBoxAlign(
                font_face->glyph,
                _surface,
                box,
                alignment::to_font_align(halign),
                value.c_str());
    }

    core::rect renderer::pop_clip_rect() {
        if (_clip_stack.empty())
            return {};
        auto rect = _clip_stack.back();
        _clip_stack.pop_back();
        _bounds = rect;
        auto sdl_rect = rect.to_sdl_rect();
        SDL_RenderSetClipRect(_surface, &sdl_rect);
        return rect;
    }

    const core::rect& renderer::bounds() const {
        return _bounds;
    }

    void renderer::sdl_renderer(SDL_Renderer* value) {
        _surface = value;
    }

    void renderer::draw_rect(const core::rect& bounds) {
        auto rect = bounds.to_sdl_rect();
        SDL_RenderDrawRect(_surface, &rect);
    }

    void renderer::fill_rect(const core::rect& bounds) {
        auto rect = bounds.to_sdl_rect();
        SDL_RenderFillRect(_surface, &rect);
    }

    void renderer::push_blend_mode(SDL_BlendMode mode) {
        SDL_BlendMode previous_blend_mode;
        SDL_GetRenderDrawBlendMode(_surface, &previous_blend_mode);
        _mode_stack.push(previous_blend_mode);
        SDL_SetRenderDrawBlendMode(_surface, mode);
    }

    void renderer::set_clip_rect(const core::rect& bounds) {
        auto sdl_rect = bounds.to_sdl_rect();
        SDL_RenderSetClipRect(_surface, &sdl_rect);
    }

    void renderer::push_clip_rect(const core::rect& bounds) {
        SDL_Rect rect {};
        SDL_RenderGetClipRect(_surface, &rect);
        _clip_stack.emplace_back(rect);
        SDL_Rect clip_rect = bounds.to_sdl_rect();
        SDL_RenderSetClipRect(_surface, &clip_rect);
        _bounds = bounds;
    }

    void renderer::draw_line(int x1, int y1, int x2, int y2) {
        SDL_RenderDrawLine(_surface, x1, y1, x2, y2);
    }

    void renderer::fill_polygon(const vertex_list& vertices) {
        auto vertex_count = static_cast<int16_t>(vertices.size());
        if (vertex_count < 3)
            return;

        auto min_y = vertices[0].y;
        auto max_y = vertices[0].y;

        for (const auto& v : vertices) {
            if (v.y < min_y)
                min_y = v.y;
            else if (v.y > max_y)
                max_y = v.y;
        }


        int16_t y1 = 0, y2 = 0;
        int16_t x1 = 0, x2 = 0;
        int16_t ind1 = 0, ind2 = 0;

        for (int16_t y = min_y; y <= max_y; y++) {
            std::vector<int32_t> points {};

            for (int16_t i = 0; i < vertex_count; i++) {
                if (i == 0) {
                    ind1 = static_cast<int16_t>(vertex_count - 1);
                    ind2 = 0;
                } else {
                    ind1 = static_cast<int16_t>(i - 1);
                    ind2 = i;
                }
                y1 = vertices[ind1].y;
                y2 = vertices[ind2].y;
                if (y1 < y2) {
                    x1 = vertices[ind1].x;
                    x2 = vertices[ind2].x;
                } else if (y1 > y2) {
                    y2 = vertices[ind1].y;
                    y1 = vertices[ind2].y;
                    x2 = vertices[ind1].x;
                    x1 = vertices[ind2].x;
                } else {
                    continue;
                }
                if (((y >= y1) && (y < y2))
                ||  ((y == max_y) && (y > y1) && (y <= y2))) {
                    auto point = ((65536 * (y - y1)) / (y2 - y1)) * (x2 - x1) + (65536 * x1);
                    points.push_back(point);
                }
            }

            std::sort(
                    points.begin(),
                    points.end(),
                    [](int32_t left, int32_t right) {
                        return left < right;
                    });

            for (size_t i = 0; (i < points.size()); i += 2) {
                auto xa = points[i] + 1;
                xa = (xa >> 16) + ((xa & 32768) >> 15);
                auto xb = points[i + 1] - 1;
                xb = (xb >> 16) + ((xb & 32768) >> 15);
                draw_line(xa, y, xb, y);
            }
        }
    }

    void renderer::set_color(const core::palette_entry& color) {
        SDL_SetRenderDrawColor(
            _surface,
            color.red(),
            color.green(),
            color.blue(),
            color.alpha());
    }

    void renderer::draw_dashed_hline(int x1, int x2, int y, uint16_t width) {
        auto length = x2 - x1;
        auto segment_length = width * 2;
        while (length > 0) {
            SDL_RenderDrawLine(_surface, x1, y, x1 + width, y);
            x1 += segment_length;
            length -= segment_length;
        }
    }

    void renderer::draw_dashed_vline(int y1, int y2, int x, uint16_t width) {
        auto height = y2 - y1;
        auto segment_length = width * 2;
        while (height > 0) {
            SDL_RenderDrawLine(_surface, x, y1, x, y1 + width);
            y1 += segment_length;
            height -= segment_length;
        }
    }

    void renderer::draw_dashed_rect(const core::rect& bounds, uint16_t width) {
        draw_dashed_hline(bounds.left(), bounds.right(), bounds.top(), width);
        draw_dashed_hline(bounds.left(), bounds.right(), bounds.bottom(), width);
        draw_dashed_vline(bounds.top(), bounds.bottom(), bounds.left(), width);
        draw_dashed_vline(bounds.top(), bounds.bottom(), bounds.right(), width);
    }

    int renderer::measure_text(const font_t* font_face, const std::string& value) {
        return FC_GetWidth(font_face->glyph, value.c_str());
    }

    void renderer::draw_selection_rect(core::rect& rect, core::palette_entry color) {
        push_blend_mode(SDL_BLENDMODE_BLEND);
        color.alpha(0x7f);
        set_color(color);
        fill_rect(rect);
        pop_blend_mode();
    }

    void renderer::draw_selection_band(core::rect& rect, core::palette_entry color) {
        set_color(color);
        draw_dashed_rect(rect, 4);
    }

}