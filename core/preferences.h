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

#include <boost/filesystem.hpp>
#include "rect.h"
#include "result.h"
#include "palette.h"
#include "font_family.h"

namespace ryu::core {

    namespace fs = boost::filesystem;

    class preferences {
    public:
        preferences() = default;

        bool save(
                core::result& result,
                const core::engine& engine);

        int32_t font_size() const;

        void font_size(int32_t value);

        fs::path default_path() const;

        bool load(core::result& result);

        core::palette ide_palette() const;

        core::rect window_position() const;

        std::string font_family_name() const;

        void default_path(const fs::path& value);

        void ide_palette(const core::palette& value);

        void window_position(const core::rect& value);

        void font_family_name(const std::string& value);

    private:
        int32_t _font_size {};
        fs::path _default_path {};
        core::palette _ide_palette {};
        core::rect _window_position {};
        std::string _font_family_name {};
    };

};

