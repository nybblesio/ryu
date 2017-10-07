//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "rect.h"

namespace ryu::core {

    rect::rect(
            int32_t left,
            int32_t top,
            int32_t width,
            int32_t height) : _top(top),
                              _left(left),
                              _width(width),
                              _height(height) {
    }

    rect::rect(const SDL_Rect& sdl_rect) {
        _left = sdl_rect.x;
        _top = sdl_rect.y;
        _width = sdl_rect.w;
        _height = sdl_rect.h;
    }

    int32_t rect::top() const {
        return _top;
    }

    void rect::top(int32_t y) {
        _top = y;
    }

    int32_t rect::left() const {
        return _left;
    }

    void rect::left(int32_t x) {
        _left = x;
    }

    void rect::width(int32_t w) {
        _width = w;
    }

    int32_t rect::width() const {
        return _width;
    }

    int32_t rect::right() const {
        return _left + _width;
    }

    int32_t rect::height() const {
        return _height;
    }

    void rect::height(int32_t h) {
        _height = h;
    }

    int32_t rect::bottom() const {
        return _top + _height;
    }

    rect& rect::pos(int32_t left, int32_t top) {
        _left = left;
        _top = top;
        return *this;
    }

    void rect::inflate(int32_t dx, int32_t dy) {
        _left -= dx;
        _width += dx;
        _top -= dy;
        _height += dy;
    }

    void rect::deflate(int32_t dx, int32_t dy) {
        _left += dx;
        _width -= dx;
        _top += dy;
        _height -= dy;
    }

    bool rect::contains(const rect& rect) const {
        return rect.right() < right()
               && rect.left() > left()
               && rect.top() > top()
               && rect.bottom() < bottom();
    }

    bool rect::intersects(const rect& rect) const {
        return    left() <= rect.right()
                  && right() >= rect.left()
                  && top() >= rect.bottom()
                  && bottom() <= rect.top();
    }

    rect& rect::size(int32_t width, int32_t height) {
        _width = width;
        _height = height;
        return *this;
    }

    bool rect::contains(int32_t x, int32_t y) const {
        return x < right() && x > left() && y < bottom() && y > top();
    }

    rect rect::center_inside(const rect& target, bool scale) {
        int32_t width = _width;
        int32_t height = _height;
        int32_t aspect_ratio = width / height;

        int32_t target_width = target.width();
        int32_t target_height = target.height();
        int32_t target_aspect_ratio = target_width / target_height;

        int32_t new_height = height;
        int32_t new_width = width;

        int32_t xstart = _left;
        int32_t ystart = _top;

        if (target_aspect_ratio > aspect_ratio) {
            if (scale) {
                new_width = target_width * (1 / (target_height / height));
                xstart = _left - ((new_width / 2) - (width / 2));
            } else {
                new_height = (int) (width / target_width * target_height);
                ystart = ((height - new_height) / 2) + _top;
            }

        } else if (target_aspect_ratio < aspect_ratio) {
            if (scale) {
                new_height = target_height * (1 / (target_width / width));
                ystart = _top - ((new_height / 2) - (height / 2));
            } else {
                new_width = (height / target_height * target_width);
                xstart = ((width - new_width) / 2) + _left;
            }
        }

        return rect(xstart, ystart, new_width, new_height);
    }

}