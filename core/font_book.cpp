//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "font_book.h"

namespace ryu::core {

    font_book* font_book::instance() {
        static font_book instance;
        return &instance;
    }

    bool font_book::load(
            core::result& result,
            const boost::filesystem::path& path) {
        // XXX: need to load multiple sizes for user config
        auto topaz = add_font_family("topaz", 14);
        topaz->add_style(font::styles::normal, "assets/fonts/topaz/Topaz-8.ttf");

        auto hack = add_font_family("hack", 16);
        hack->add_style(font::styles::normal,                      "assets/fonts/hack/Hack-Regular.ttf");
        hack->add_style(font::styles::bold,                        "assets/fonts/hack/Hack-Bold.ttf");
        // XXX: there appears to be an issue with this font
        hack->add_style(font::styles::italic,                      "assets/fonts/hack/Hack-Italic.ttf");
        hack->add_style(font::styles::underline,                   "assets/fonts/hack/Hack-Regular.ttf");
        hack->add_style(font::styles::bold|font::styles::underline,"assets/fonts/hack/Hack-Bold.ttf");
        hack->add_style(font::styles::bold|font::styles::italic,   "assets/fonts/hack/Hack-BoldItalic.ttf");

        return true;
    }

    core::font_family* font_book::add_font_family(
            const std::string& name,
            uint32_t size) {
        auto it = _font_families.insert(std::make_pair(
                fmt::format("{}:{}", name, size),
                font_family(name, size, _renderer)));
        return &(*it.first).second;
    }

    core::font_family* font_book::find_font_family(
            const std::string& name,
            uint32_t size) {
        auto it = _font_families.find(fmt::format("{}:{}", name, size));
        if (it == _font_families.end())
            return nullptr;
        return &it->second;
    }

    void font_book::renderer(SDL_Renderer* value) {
        _renderer = value;
    }

}