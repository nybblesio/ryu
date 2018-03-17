//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <core/input_action.h>
#include "column_pick_list.h"

namespace ryu::core {

    column_pick_list::column_pick_list(
            const std::string& name,
            core::view_host* host) : core::view(types::control, name, host) {
    }

    void column_pick_list::clear() {
        _headers.clear();
        _rows.clear();
    }

    bool column_pick_list::move_up() {
        if (_selected == 0) {
            return move_row_up();
        }
        _selected--;
        return true;
    }

    void column_pick_list::add_header(
            const std::string& title,
            uint8_t fg_color,
            uint8_t bg_color,
            uint32_t width,
            halign_t halign,
            valign_t valign) {
        _headers.emplace_back(
            title,
            fg_color,
            bg_color,
            width,
            valign,
            halign);
    }

    bool column_pick_list::move_down() {
        _selected++;
        auto max = std::min<uint32_t>(
            _visible_rows,
            static_cast<const uint32_t&>(_rows.size())) - 1;
        if (_selected > max) {
            _selected = max;
            return move_row_down();
        }
        return true;
    }

    void column_pick_list::clear_rows() {
        _rows.clear();
        raise_selection_changed();
    }

    bool column_pick_list::move_row_up() {
        if (_row == 0) {
            return false;
        }
        _row--;
        return true;
    }

    bool column_pick_list::move_row_down() {
        if (_rows.size() < _visible_rows)
            return false;
        _row++;
        auto max = _rows.size() - _visible_rows;
        if (_row > max) {
            _row = static_cast<int>(max);
            return false;
        }
        return true;
    }

    void column_pick_list::on_initialize() {
        auto up_action = core::input_action::create_no_map(
            "column_pick_list_up_action",
            "Internal",
            "Move to the previous pick list item.");
        up_action->register_handler(
            action_sink::types::view,
            [this](const event_data_t& data) {
                return focused();
            },
            [this](const event_data_t& data) {
                if (move_up())
                    raise_selection_changed();
                return true;
            });
        up_action->bind_keys({core::key_up});

        auto down_action = core::input_action::create_no_map(
            "column_pick_list_down_action",
            "Internal",
            "Move to the next pick list item.");
        down_action->register_handler(
            action_sink::types::view,
            [this](const event_data_t& data) {
                return focused();
            },
            [this](const event_data_t& data) {
                if (move_down())
                    raise_selection_changed();
                return true;
            });
        down_action->bind_keys({core::key_down});

        auto select_action = core::input_action::create_no_map(
            "column_pick_list_select_action",
            "Internal",
            "Make current item the selected value.");
        select_action->register_handler(
            action_sink::types::view,
            [this](const event_data_t& data) {
                return focused();
            },
            [this](const event_data_t& data) {
                raise_activated();
                return true;
            });
        select_action->bind_keys({core::key_return});
    }

    void column_pick_list::raise_activated() {
        if (_activated_callable != nullptr)
            _activated_callable(_row + _selected);
    }

    uint32_t column_pick_list::selected() const {
        return _selected;
    }

    const row_list& column_pick_list::rows() const {
        return _rows;
    }

    border::types column_pick_list::border() const {
        return _border;
    }

    void column_pick_list::raise_selection_changed() {
        if (_selection_changed_callable != nullptr) {
            if (_rows.empty())
                _selection_changed_callable(-1);
            else
                _selection_changed_callable(_row + _selected);
        }
    }

    void column_pick_list::remove_row(uint32_t index) {
        if (index < _rows.size()) {
            _rows.erase(_rows.begin() + index);
            if (_selected == index) {
                move_up();
                return;
            }
            raise_selection_changed();
        }
    }

    void column_pick_list::border(border::types value) {
        _border = value;
    }

    void column_pick_list::on_draw(core::renderer& surface) {
        auto pal = *palette();
        auto fg = adjust_color(pal[fg_color()]);
        auto& bg = pal[bg_color()];

        surface.set_color(bg);

        auto bounds = client_bounds();
        surface.fill_rect(bounds);
        surface.push_clip_rect(client_bounds());

        if (_border == border::types::solid) {
            surface.set_color(fg);
            surface.draw_rect(bounds);
        }

        auto row_height = font_face()->line_height + 2;

        auto header_x = bounds.left();
        for (const auto& header : _headers) {
            core::rect header_rect {
                header_x,
                bounds.top(),
                header.width,
                row_height};
            surface.set_color(adjust_color(pal[header.bg_color]));
            surface.fill_rect(header_rect);
            surface.set_font_color(
                font_face(),
                adjust_color(pal[header.fg_color]));
            header_rect.deflate(3, 2);
            surface.draw_text_aligned(
                font_face(),
                header.title,
                header_rect,
                header.halign,
                header.valign);
            header_x += header.width + 2;
        }

        auto column_y = (bounds.top() + row_height) + (row_height / 2);
        for (auto row_index = _row;
             row_index < _row + _max_rows;
             row_index++) {
            const auto& row = _rows[row_index];

            if (row_index == _row + _selected) {
                surface.push_blend_mode(SDL_BLENDMODE_BLEND);
                auto selection_color = fg;
                selection_color.alpha(0x5f);
                surface.set_color(selection_color);
                surface.fill_rect({2, column_y, bounds.width() + 5, row_height});
                surface.pop_blend_mode();
            }

            auto index = 0;
            auto column_x = bounds.left() + 2;
            for (const auto& header : _headers) {
                auto clamped_width = column_x + header.width > bounds.right() ?
                                     bounds.right() - column_x :
                                     header.width;
                core::rect column_rect {
                    column_x,
                    column_y,
                    clamped_width,
                    row_height};
                surface.push_clip_rect(column_rect);
                surface.set_font_color(
                    font_face(),
                    adjust_color(pal[header.fg_color]));
                surface.draw_text_aligned(
                    font_face(),
                    row.columns[index++],
                    column_rect,
                    header.halign,
                    header.valign);
                surface.pop_clip_rect();
                column_x += header.width + 2;
            }

            column_y += row_height;
        }

        surface.pop_clip_rect();
    }

    void column_pick_list::add_row(const pick_list_row_t& row) {
        _rows.push_back(row);
        raise_selection_changed();
    }

    palette_entry column_pick_list::adjust_color(palette_entry entry) {
        if (!enabled() || !focused()) {
            return entry - 35;
        }
        return entry;
    }

    void column_pick_list::on_resize(const core::rect& context_bounds) {
        view::on_resize(context_bounds);

        auto row_height = font_face()->line_height + 2;
        auto vertical_margin = (margin().top() + margin().bottom());
        _visible_rows = static_cast<uint32_t>((client_bounds().height() - vertical_margin) / row_height);
        if (_visible_rows > 0 && _rows.size() > 1)
            _visible_rows--;
        _max_rows = std::min<uint32_t>(
            _visible_rows,
            static_cast<uint32_t>(_rows.size()));
    }

    void column_pick_list::on_activated(const column_pick_list::activated_callable& callable) {
        _activated_callable = callable;
    }

    void column_pick_list::on_selection_changed(const column_pick_list::selection_changed_callable& callable) {
        _selection_changed_callable = callable;
    }

}