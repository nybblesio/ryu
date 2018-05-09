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
#include "caret.h"

namespace ryu::core {

    class palette_entry_editor : public core::view {
    public:
        palette_entry_editor(
            const std::string& name,
            core::view_host* host);

        ~palette_entry_editor() override;

        bool locked() const;

        palette_index entry();

        void locked(bool value);

        void entry(palette_index value);

        void text_color(palette_index value);

    protected:
        void bind_events();

        void define_actions();

        void on_initialize() override;

        void on_focus_changed() override;

        void on_palette_changed() override;

        void on_font_family_changed() override;

        void on_draw(core::renderer& surface) override;

    private:
        void caret_left();

        void update_minimum_size();

        void caret_right(uint8_t overflow_column = 2);

    private:
        core::caret _caret;
        bool _locked = false;
        palette_index _index = 0;
        palette_index _text_color;
    };

};
