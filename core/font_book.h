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

#include <string>
#include "font_family.h"
#include "result.h"

namespace ryu::core {

    class font_book {
    public:
        static font_book* instance();

        void renderer(SDL_Renderer* value);

        bool load(
                core::result& result,
                const boost::filesystem::path& path);

        core::font_family* add_font_family(
                const std::string& name,
                uint32_t size);

        core::font_family* find_font_family(
                const std::string& name,
                uint32_t size);

    private:
        font_family_dict _font_families;
        SDL_Renderer* _renderer = nullptr;
    };

};

