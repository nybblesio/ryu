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

#include <map>
#include <string>
#include <common/SDL_FontCache.h>
#include <boost/filesystem/path.hpp>

namespace ryu::core {

    struct font {
        enum styles : uint8_t {
            normal        = 0b00000000,
            italic        = 0b00000001,
            bold          = 0b00000010,
            underline     = 0b00000100,
            strikethrough = 0b00001000,
        };

        enum flags : uint8_t {
            none          = 0b00000000,
            reverse       = 0b00000001,
            link          = 0b00000010,
        };

        static int to_sdl_style(uint8_t flags) {
            int sdl = TTF_STYLE_NORMAL;

            if ((flags & styles::normal) != 0)
                sdl |= TTF_STYLE_NORMAL;
            if ((flags & styles::italic) != 0)
                sdl |= TTF_STYLE_ITALIC;
            if ((flags & styles::bold) != 0)
                sdl |= TTF_STYLE_BOLD;
            if ((flags & styles::underline) != 0)
                sdl |= TTF_STYLE_UNDERLINE;
            if ((flags & styles::strikethrough) != 0)
                sdl |= TTF_STYLE_STRIKETHROUGH;

            return sdl;
        }
    };

    struct font_t {
        int32_t width {};
        int32_t size {};
        int32_t line_height {};
        FC_Font* glyph = nullptr;

        int measure_chars(int count) const {
            return width * count;
        }

        int measure_text(const std::string& value) const {
            return FC_GetWidth(glyph, value.c_str());
        }
    };

    class font_family {
    public:
        font_family(
                const std::string& name,
                int32_t size,
                SDL_Renderer* renderer);

        ~font_family();

        font_t* add_style(
                uint8_t style,
                const boost::filesystem::path& path);

        int32_t size() const {
            return _size;
        }

        std::string name() const {
            return _name;
        }

        const font_t* find_style(uint8_t style) const;

    private:
        int32_t _size;
        std::string _name;
        std::map<uint8_t, font_t> _faces;
        SDL_Renderer* _renderer = nullptr;
    };

    typedef std::map<std::string, font_family> font_family_dict;

};

