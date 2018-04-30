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

        void value(const std::string& text) override;

    protected:
        bool page_up();

        bool move_up();

        bool move_down();

        bool page_down();

        bool move_row_up();

        bool move_row_down();

        void on_initialize() override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const rect& context_bounds) override;

    private:
        void bind_events();

        void define_actions();

    private:
        int _row = 0;
        int _width = 0;
        int _height = 0;
        int _length = 32;
        int _selection = 0;
        std::string _search;
        option_list _options {};
        int _visibile_items = 10;
        border::types _border = border::types::none;
    };

};

