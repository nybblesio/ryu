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

    class memory_editor : public core::view {
    public:
        using caret_changed_callable = std::function<void(const core::caret&)>;

        memory_editor(
            const std::string& name,
            core::view_host* host);

        ~memory_editor() override;

        void clear();

        int page_width() const {
            return _metrics.page_width;
        }

        int page_height() const {
            return _metrics.page_height;
        }

        void address(uint32_t address);

        void caret_color(palette_index value);

        void address_color(palette_index value);

        void selection_color(palette_index value);

        void on_caret_changed(const caret_changed_callable& callable);

    protected:
        struct metrics_t {
            int address_width {};
            uint8_t page_width {};
            uint8_t page_height {};
            const int left_padding = 10;
            const int right_padding = 10;
        };

        void raise_caret_changed();

        void on_initialize() override;

        void on_bounds_changed() override;

        void on_draw(core::renderer& surface) override;

    private:
        void caret_left();

        void bind_events();

        void define_actions();

        void calculate_page_metrics();

        void on_focus_changed() override;

        void caret_right(uint8_t overflow_column = 0);

    private:
        uint32_t _address;
        core::caret _caret;
        metrics_t _metrics;
        uint8_t _nybble = 0;
        palette_index _address_color;
        palette_index _selection_color;
        caret_changed_callable _caret_changed_callback;
    };

};

