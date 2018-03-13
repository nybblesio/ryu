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
#include <core/selection.h>
#include "caret.h"

namespace ryu::core {

    class memory_editor : public core::view {
    public:
        using caret_changed_callable = std::function<void(const core::caret&)>;

        memory_editor(
            const std::string& name,
            core::view_container* container);

        void clear();

        int page_width() const {
            return _metrics.page_width;
        }

        int page_height() const {
            return _metrics.page_height;
        }

        void caret_color(uint8_t value);

        void address_color(uint8_t value);

        void goto_address(uint32_t address);

        void selection_color(uint8_t value);

        void find(const std::string& needle);

        void initialize(uint32_t rows, uint16_t columns);

        void on_caret_changed(const caret_changed_callable& callable);

    protected:
        struct metrics_t {
            uint8_t page_width {};
            uint8_t page_height {};
            int line_number_width {};
            const int left_padding = 10;
            const int right_padding = 10;
        };

        void raise_caret_changed();

        void on_draw(core::renderer& surface) override;

        void on_resize(const core::rect& context_bounds) override;

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

        bool scroll_right();

        void end_selection();

        void delete_selection();

        void calculate_page_metrics();

        void update_virtual_position();

        void caret_up(uint8_t rows = 1);

        void on_focus_changed() override;

        void caret_down(uint8_t rows = 1);

        void caret_left(uint8_t columns = 1);

        bool caret_right(uint8_t columns = 1);

        void update_selection(uint16_t line_end);

    private:
        uint16_t _vcol;
        uint32_t _vrow;
        core::caret _caret;
        metrics_t _metrics;
        uint8_t _selection_color;
        uint8_t _line_number_color;
        core::selection _selection;
        caret_changed_callable _caret_changed_callback;
    };

};

