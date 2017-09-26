//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "font_family.h"

namespace ryu::core {

    font_family::font_family(
            std::string name,
            int32_t size,
            SDL_Renderer* renderer) : _size(size),
                                      _name(std::move(name)),
                                      _renderer(renderer) {
    }

    font_family::~font_family() {
        for (auto& it : _faces)
            FC_FreeFont(it.second.glyph);
    }

    const font_t* font_family::find_style(uint8_t style) const {
        auto it = _faces.find(style);
        if (it == _faces.end()) {
            it = _faces.find(font::styles::normal);
            if (it == _faces.end())
                return nullptr;
        }
        return &it->second;
    }

    font_t* font_family::add_style(uint8_t style, const boost::filesystem::path& path) {
        auto instance = FC_CreateFont();
        auto result = FC_LoadFont(
                instance,
                _renderer,
                path.c_str(),
                static_cast<Uint32>(_size),
                FC_MakeColor(0xff, 0xff, 0xff, 0xff),
                font::to_sdl_style(style));
        if (result != 0) {
            auto it = _faces.insert(std::make_pair(
                    style,
                    font_t{FC_GetWidth(instance, "A"), _size, FC_GetLineHeight(instance), instance}));
            return &(*it.first).second;
        }
        return nullptr;
    }

}