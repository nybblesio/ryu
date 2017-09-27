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

#include <SDL_system.h>
#include <core/context.h>
#include <core/view.h>

namespace ryu::core {

    class caret : public core::view {
    public:
        struct mode {
            enum types {
                insert = 1,
                overwrite,
                select,
                column_select
            };
        };

        enum ids {
            blink_timer = 200
        };

        caret(
                core::context* context,
                core::view* parent,
                int id,
                const std::string& name);

        ~caret() override;

        void select();

        void insert();

        void overwrite();

        void column_select();

        bool up(int rows);

        bool down(int rows);

        int row() const;

        bool row(int row);

        bool right(int columns);

        bool left(int columns);

        int column() const;

        bool column(int column);

        mode::types mode() const;

        void initialize(int row, int column);

        void page_size(int page_height, int page_width);

    protected:
        void on_draw() override;

    private:
        bool clamp_row();

        bool clamp_column();

    private:
        int _row = 0;
        int _column = 0;
        int _page_width = 0;
        int _page_height = 0;
        timer* _timer = nullptr;
        mode::types _mode = mode::types::insert;
    };

};

