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

#include <core/view.h>
#include <core/document.h>
#include "caret.h"

namespace ryu::core {

    class text_box : public core::view {
    public:
        using on_key_down_callable = std::function<bool (int)>;

        text_box(
            const std::string& name,
            core::view_host* host);

        ~text_box() override;

        void clear();

        uint16_t width() const;

        uint16_t length() const;

        void width(uint8_t value);

        void length(uint16_t value);

        std::string value() const override;

        void bg_color(palette_index value) override;

        void fg_color(palette_index value) override;

        void palette(core::palette* value) override;

        void value(const std::string& value) override;

        void font_family(core::font_family* value) override;

        void on_key_down(const on_key_down_callable& callable);

    protected:
        void caret_end();

        void caret_home();

        void on_initialize() override;

        void on_focus_changed() override;

        bool caret_left(uint8_t columns = 1);

        bool caret_right(uint8_t columns = 1);

        void on_draw(core::renderer& surface) override;

    private:
        void bind_events();

        void define_actions();

        void update_minimum_size();

    private:
        core::caret _caret;
        core::document _document;
        on_key_down_callable _on_key_down {};
    };

};

