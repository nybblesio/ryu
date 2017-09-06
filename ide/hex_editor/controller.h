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

#include <core/state.h>
#include <core/caret.h>

namespace ryu::ide::hex_editor {

    class controller : public core::state {
    public:
        enum ids {
            caret_blink_timer = 3
        };

        controller(
                core::context* context,
                int id,
                const std::string& name);

        ~controller() override;

    protected:
        void on_update(uint32_t dt) override;

        void on_draw(SDL_Renderer* renderer) override;

        void on_init(SDL_Renderer* renderer) override;

        bool on_process_event(const SDL_Event* e) override;

    private:
        void page_up();

        void last_page();

        void page_down();

        void caret_end();

        void scroll_up();

        void caret_home();

        void first_page();

        void scroll_left();

        void scroll_down();

        void scroll_right();

        void end_selection();

        void update_selection();

        void caret_up(int rows = 1);

        void caret_down(int rows = 1);

        void update_virtual_position();

        void caret_left(int columns = 1);

        void caret_right(int columns = 1);

    private:
        int _vcol = 0;
        int _vrow = 0;
        int _top = 47;
        int _left = 66;
        int _page_width = 66;
        int _page_height = 30;
        core::font_t* _font = nullptr;
        ryu::core::caret* _caret = nullptr;
        std::pair<int, int> _selection_end = {-1, -1};
        std::pair<int, int> _selection_start = {-1, -1};
    };

};

