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

#include <yaml-cpp/yaml.h>
#include <boost/filesystem.hpp>
#include "rect.h"
#include "result.h"
#include "palette.h"
#include "font_family.h"

namespace ryu::core {

    namespace fs = boost::filesystem;

    using font_value_t = std::pair<std::string, uint32_t>;

    class preferences {
    public:
        preferences() = default;

        bool full_screen() const;

        void full_screen(bool value);

        fs::path default_path() const;

        font_value_t ide_font() const;

        bool save(core::result& result);

        bool load(core::result& result);

        fs::path font_book_path() const;

        font_value_t engine_font() const;

        fs::path executable_path() const;

        core::palette ide_palette() const;

        font_value_t emulator_font() const;

        core::rect window_position() const;

        fs::path preferences_file_path() const;

        void default_path(const fs::path& value);

        void ide_font(const font_value_t& value);

        void font_book_path(const fs::path& value);

        void engine_font(const font_value_t& value);

        void executable_path(const fs::path& value);

        void ide_palette(const core::palette& value);

        void emulator_font(const font_value_t& value);

        void window_position(const core::rect& value);

        core::context_window::sizes ide_window_size() const;

        void ide_window_size(core::context_window::sizes size);

        core::context_window::sizes emulator_window_size() const;

        void emulator_window_size(core::context_window::sizes size);

    private:
        bool get_font_family_and_size(YAML::Node& node, font_value_t& value);

    private:
        bool _full_screen = false;
        fs::path _default_path {};
        fs::path _executable_path {};
        core::palette _ide_palette {};
        core::rect _window_position {};
        font_value_t _ide_font {"hack", 16};
        font_value_t _engine_font {"topaz", 16};
        font_value_t _emulator_font {"topaz", 16};
        fs::path _font_book_path {"assets/fonts/font_book.yaml"};
        core::context_window::sizes _ide_window_size = core::context_window::sizes::split;
        core::context_window::sizes _emulator_window_size = core::context_window::sizes::split;
    };

};

