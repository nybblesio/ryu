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

    class pick_list : public core::view {
    public:
        pick_list(
            const std::string& name,
            core::view_host* host);

        ~pick_list() override;

        int width() const;

        int height() const;

        int length() const;

        void reset_search();

        void width(int value);

        void height(int value);

        option_list& options();

        void length(int value);

        int visible_items() const;

        border::types border() const;

        void visible_items(int value);

        void selected_key(uint32_t key);

        void border(border::types value);

        std::string value() const override;

        void row_color(palette_index value);

        void selection_color(palette_index value);

        void not_found_color(palette_index value);

        void value(const std::string& text) override;

    protected:
        bool page_up();

        bool move_up();

        void move_top();

        bool move_down();

        bool page_down();

        bool move_row_up();

        bool move_row_down();

        void on_initialize() override;

        void on_focus_changed() override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const rect& context_bounds) override;

    private:
        void bind_events();

        void define_actions();

        bool find_matching_text(const std::string& text);

    private:
        bool _found;
        int _row = 0;
        int _width = 0;
        int _height = 0;
        int _length = 32;
        int _selection = 0;
        core::caret _caret;
        std::string _search;
        int _selected_item = 0;
        option_list _options {};
        int _visible_items = 10;
        palette_index _row_color;
        palette_index _selection_color;
        palette_index _not_found_color;
        border::types _border = border::types::none;
    };

};

