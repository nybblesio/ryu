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

    fs::path preferences::default_path() const {
        return _default_path;
    }

    font_value_t preferences::ide_font() const {
        return _ide_font;
    }

    bool preferences::load(core::result& result) {
        return true;
    }

    fs::path preferences::font_book_path() const {
        return _font_book_path;
    }

    font_value_t preferences::engine_font() const {
        return _engine_font;
    }

    core::palette preferences::ide_palette() const {
        return _ide_palette;
    }

    font_value_t preferences::emulator_font() const {
        return _emulator_font;
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

    void preferences::default_path(const fs::path& value) {
        _default_path = value;
    }

    void preferences::ide_font(const font_value_t& value) {
        _ide_font = value;
    }

    void preferences::font_book_path(const fs::path& value) {
        _font_book_path = value;
    }

    void preferences::engine_font(const font_value_t& value) {
        _engine_font = value;
    }

    void preferences::ide_palette(const core::palette& value) {
        _ide_palette = value;
    }

    void preferences::window_position(const core::rect& value) {
        _window_position = value;
    }

    void preferences::emulator_font(const font_value_t& value) {
        _emulator_font = value;
    }

}