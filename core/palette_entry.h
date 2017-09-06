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
#include <SDL_pixels.h>

namespace ryu::core {

    class palette_entry {
    public:
        palette_entry() = default;

        palette_entry(
                uint8_t red,
                uint8_t green,
                uint8_t blue,
                uint8_t alpha);

        uint8_t red() const;

        uint8_t green() const;

        uint8_t blue() const;

        uint8_t alpha() const;

        void red(uint8_t value);

        void blue(uint8_t value);

        void green(uint8_t value);

        void alpha(uint8_t value);

        palette_entry fade(uint8_t factor);

        inline SDL_Color to_sdl_color() {
            return SDL_Color{_red, _green, _blue, _alpha};
        }

    private:
        uint8_t _red {};
        uint8_t _green {};
        uint8_t _blue {};
        uint8_t _alpha {};
    };

};

