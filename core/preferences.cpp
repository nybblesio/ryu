//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "preferences.h"

namespace ryu::core {

    bool preferences::save(
            core::result& result,
            const core::engine& engine) {
        return true;
    }

    int32_t preferences::font_size() const {
        return _font_size;
    }

    void preferences::font_size(int32_t value) {
        _font_size = value;
    }

    fs::path preferences::default_path() const {
        return _default_path;
    }

    bool preferences::load(core::result& result) {
        return true;
    }

    core::palette preferences::ide_palette() const {
        return _ide_palette;
    }

    core::rect preferences::window_position() const {
        if (_window_position.empty())
            return {
                    SDL_WINDOWPOS_CENTERED,
                    SDL_WINDOWPOS_CENTERED,
                    1920,
                    1080
            };
        return _window_position;
    }

    std::string preferences::font_family_name() const {
        return _font_family_name;
    }

    void preferences::default_path(const fs::path& value) {
        _default_path = value;
    }

    void preferences::ide_palette(const core::palette& value) {
        _ide_palette = value;
    }

    void preferences::window_position(const core::rect& value) {
        _window_position = value;
    }

    void preferences::font_family_name(const std::string& value) {
        _font_family_name = value;
    }

}