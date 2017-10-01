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

    typedef std::vector<std::string> option_list;

    class pick_list : public core::view {
    public:
        explicit pick_list(const std::string& name);

        int length() const;

        std::string value();

        option_list& options();

        void length(int value);

        int visible_items() const;

        border::types border() const;

        void visible_items(int value);

        void border(border::types value);

        void value(const std::string& value);

    protected:
        bool move_up();

        bool move_down();

        bool move_row_up();

        bool move_row_down();

        void on_draw(SDL_Renderer* renderer) override;

        bool on_process_event(const SDL_Event* e) override;

        void on_resize(const rect& context_bounds) override;

    private:
        int _row = 0;
        int _length = 32;
        std::string _value;
        int _selection = 0;
        option_list _options {};
        int _visibile_items = 10;
        border::types _border = border::types::none;
    };

};

