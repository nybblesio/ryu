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
#include <core/project.h>
#include <core/document.h>
#include <core/selection.h>
#include <core/input_action.h>
#include "caret.h"

namespace ryu::core {

    namespace fs = boost::filesystem;

    class text_editor : public core::view {
    public:
        using caret_changed_callable = std::function<void(const core::caret&, const core::document&)>;
        using char_action_callable = std::function<void (uint32_t, uint16_t)>;

        text_editor(
                const std::string& name,
                core::view_host* host);

        void clear();

        fs::path path() const {
            return _document.path();
        }

        int page_width() const {
            return _metrics.page_width;
        }

        int page_height() const {
            return _metrics.page_height;
        }

        void goto_line(uint32_t row);

        void caret_color(uint8_t value);

        void path(const fs::path& value) {
            _document.path(value);
        }

        void selection_color(uint8_t value);

        void find(const std::string& needle);

        void line_number_color(uint8_t value);

        void initialize(uint32_t rows, uint16_t columns);

        bool load(core::result& result, const fs::path& path);

        bool save(core::result& result, const fs::path& path = "");

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

        void bind_events();

        void scroll_left();

        void scroll_down();

        bool scroll_right();

        void caret_newline();

        void end_selection();

        void delete_selection();

        void calculate_page_metrics();

        void update_virtual_position();

        void caret_up(uint8_t rows = 1);

        void on_focus_changed() override;

        void caret_down(uint8_t rows = 1);

        void insert_text(const char* text);

        void caret_left(uint8_t columns = 1);

        bool caret_right(uint8_t columns = 1);

        void update_selection(uint16_t line_end);

        void get_selected_text(std::stringstream& stream);

        bool input_event_filter(const core::event_data_t& data);

        void for_each_selection_char(const char_action_callable& action);

    private:
        uint16_t _vcol;
        uint32_t _vrow;
        core::caret _caret;
        metrics_t _metrics;
        core::document _document;
        uint8_t _selection_color;
        uint8_t _line_number_color;
        core::selection _selection;
        caret_changed_callable _caret_changed_callback;
    };

};

