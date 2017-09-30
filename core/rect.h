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

#include <cstdint>
#include <SDL_rect.h>
#include "padding.h"

namespace ryu::core {

    class rect {
    public:
        rect() = default;

        rect(int32_t left, int32_t top, int32_t width, int32_t height);

        bool empty() const {
            return _top == 0 && _left == 0 && _width == 0 && _height == 0;
        }

        int32_t top() const;

        void top(int32_t y);

        int32_t left() const;

        void left(int32_t x);

        int32_t width() const;

        void width(int32_t w);

        int32_t right() const;

        int32_t height() const;

        void height(int32_t h);

        int32_t bottom() const;

        rect& pos(int32_t left, int32_t top);

        void inflate(int32_t dx, int32_t dy);

        void deflate(int32_t dx, int32_t dy);

        bool contains(const rect& rect) const;

        inline SDL_Rect to_sdl_rect() const {
            return SDL_Rect{left(), top(), width(), height()};
        }

        bool intersects(const rect& rect) const;

        rect& size(int32_t width, int32_t height);

        bool contains(int32_t x, int32_t y) const;

        rect center_inside(const rect& target, bool scale = false);

    private:
        int32_t _top {};
        int32_t _left {};
        int32_t _width {};
        int32_t _height {};
    };

};

