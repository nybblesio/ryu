//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "renderer.h"

namespace ryu::core {

    renderer::renderer(SDL_Renderer* surface) {
        _surface = surface;
    }

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

    void renderer::set_color(const core::palette_entry& color) {
        SDL_SetRenderDrawColor(_surface, color.red(), color.green(), color.blue(), color.alpha());
    }

    int renderer::measure_text(const font_t* font_face, const std::string& value) {
        return FC_GetWidth(font_face->glyph, value.c_str());
    }

}