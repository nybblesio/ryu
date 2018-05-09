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
#include <core/timer.h>
#include <core/context.h>

namespace ryu::core {

    class caret : public core::view {
    public:
        using caret_changed_callable = std::function<void ()>;

        struct mode {
            enum types {
                insert = 1,
                overwrite,
                select,
                column_select
            };
        };

        caret(
            const std::string& name,
            core::view_host* host);

        ~caret() override;

        void select();

        void insert();

        void overwrite();

        void last_column();

        uint8_t row() const;

        void column_select();

        bool row(uint8_t row);

        bool up(uint8_t rows);

        uint8_t column() const;

        bool down(uint8_t rows);

        mode::types mode() const;

        bool left(uint8_t columns);

        bool right(uint8_t columns);

        bool column(uint8_t column);

        void position(uint8_t row, uint8_t column);

        void page_size(uint8_t page_height, uint8_t page_width);

        void on_caret_changed(const caret_changed_callable& callable);

    protected:
        void raise_caret_changed();

        void on_initialize() override;

        void on_font_family_changed() override;

        void on_draw(core::renderer& surface) override;

    private:
        bool clamp_row();

        bool clamp_column();

    private:
        int16_t _row = 0;
        bool _show = true;
        int16_t _column = 0;
        core::timer _timer;
        uint8_t _page_width = 0;
        uint8_t _page_height = 0;
        mode::types _mode = mode::types::insert;
        caret_changed_callable _caret_changed_callback;
    };

};

