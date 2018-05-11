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

#include <utility>
#include <variant>
#include <core/view.h>
#include <core/core_types.h>
#include "caret.h"

namespace ryu::core {

    struct pick_list_header_t {
        using valign_t = core::alignment::vertical::types;
        using halign_t = core::alignment::horizontal::types;

        enum class types : uint8_t {
            value,
            text_box,
            check_box,
            button
        };

        enum class formats : uint8_t {
            none,
            hex2,
            hex4,
            hex8,
            binary2,
            binary4,
            binary8,
            binary16,
            binary32,
        };

        pick_list_header_t(
            std::string title,
            uint8_t fg,
            uint8_t bg,
            uint32_t width,
            formats format = formats::none,
            types type = types::value,
            valign_t valign = valign_t::middle,
            halign_t halign = halign_t::center) : title(std::move(title)),
                                                  fg_color(fg),
                                                  bg_color(bg),
                                                  width(width),
                                                  valign(valign),
                                                  halign(halign),
                                                  format(format),
                                                  type(type) {
        }

        std::string title;
        uint8_t fg_color;
        uint8_t bg_color;
        int32_t width;
        valign_t valign;
        halign_t halign;
        formats format;
        types type;
    };

    using pick_list_variant = std::variant<std::monostate, uint32_t, std::string, bool>;

    enum class pick_list_variant_types {
        empty,
        u32,
        string,
        boolean
    };

    struct pick_list_row_t {
        uint32_t key;
        std::vector<pick_list_variant> columns {};
        std::vector<std::string> formatted_columns {};
    };

    using row_list = std::vector<pick_list_row_t>;
    using header_list = std::vector<pick_list_header_t>;

    class column_pick_list : public core::view {
    public:
        using activated_callable = std::function<void (uint32_t)>;
        using selection_changed_callable = std::function<void (int32_t)>;

        using valign_t = core::alignment::vertical::types;
        using halign_t = core::alignment::horizontal::types;

        const int32_t row_height_margin = 4;

        column_pick_list(
            const std::string& name,
            core::view_host* host);

        ~column_pick_list() override;

        void clear();

        void add_header(
            const std::string& title,
            uint8_t fg_color,
            uint8_t bg_color,
            uint32_t width,
            pick_list_header_t::formats format = pick_list_header_t::formats::none,
            pick_list_header_t::types type = pick_list_header_t::types::value,
            halign_t halign = halign_t::center,
            valign_t valign = valign_t::middle);

        void clear_rows();

        void reset_search();

        uint32_t selected() const;

        const row_list& rows() const;

        void remove_row(uint32_t index);

        void row_color(palette_index value);

        void add_row(const pick_list_row_t& row);

        void selection_color(palette_index value);

        void not_found_color(palette_index value);

        void on_activated(const activated_callable& callable);

        void on_selection_changed(const selection_changed_callable& callable);

    protected:
        bool move_up();

        bool page_up();

        bool move_top();

        bool page_down();

        bool move_down();

        bool move_row_up();

        bool move_row_down();

        void raise_activated();

        void on_initialize() override;

        void raise_selection_changed();

        void on_focus_changed() override;

        void on_bounds_changed() override;

        void on_palette_changed() override;

        void on_font_family_changed() override;

        void on_draw(core::renderer& surface) override;

    private:
        void bind_events();

        void define_actions();

        void calculate_visible_and_max_rows();

        palette_entry adjust_color(palette_entry entry);

        bool find_matching_text(const std::string& text);

    private:
        bool _found;
        uint32_t _row = 0;
        row_list _rows {};
        uint32_t _page = 0;
        core::caret _caret;
        std::string _search;
        int32_t _row_height;
        uint32_t _max_page = 0;
        uint32_t _max_rows = 0;
        uint32_t _selected = 0;
        header_list _headers {};
        palette_index _row_color;
        uint32_t _visible_rows = 0;
        palette_index _selection_color;
        palette_index _not_found_color;
        activated_callable _activated_callable;
        selection_changed_callable _selection_changed_callable;
    };

};