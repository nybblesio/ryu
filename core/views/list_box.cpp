//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "list_box.h"

namespace ryu::core {

    list_box::list_box(
            const std::string& name,
            core::view_host* host) : core::view(core::view::types::control, name, host) {
    }

    list_box::~list_box() {
    }

    bool list_box::move_up() {
        _selected_item--;
        if (_selected_item < 0) {
            _selected_item = 0;
            return move_row_up();
        }
        return false;
    }

    bool list_box::move_down() {
        _selected_item++;
        if (_selected_item > _visible_rows - 1) {
            _selected_item = _visible_rows - 1;
            return move_row_down();
        }
        return false;
    }

    bool list_box::move_row_up() {
        _row--;
        if (_row < 0) {
            _row = 0;
            return true;
        }
        return false;
    }

    bool list_box::move_row_down() {
        if (_options.size() < _visible_rows)
            return false;
        _row++;
        auto max = _options.size() - _visible_rows;
        if (_row > max) {
            _row = static_cast<int>(max);
            return true;
        }
        return false;
    }

    option_list& list_box::options() {
        return _options;
    }

    void list_box::define_actions() {
        auto up_action = core::input_action::create_no_map(
            "list_box_up_action",
            "Internal",
            "Move to the previous list item.");
        if (!up_action->has_bindings())
            up_action->bind_keys({core::key_up});

        auto down_action = core::input_action::create_no_map(
            "list_box_down_action",
            "Internal",
            "Move to the next list item.");
        if (!down_action->has_bindings())
            down_action->bind_keys({core::key_down});

        auto select_action = core::input_action::create_no_map(
            "list_box_select",
            "Internal",
            "Select an item in the list box.");
        if (!select_action->has_bindings())
            select_action->bind_keys({core::key_space});
    }

    void list_box::bind_events() {
        action_provider().register_handler(
            core::input_action::find_by_name("list_box_up_action"),
            [this](const event_data_t& data) {
                move_up();
                return true;
            });
        action_provider().register_handler(
            core::input_action::find_by_name("list_box_down_action"),
            [this](const event_data_t& data) {
                move_down();
                return true;
            });
        action_provider().register_handler(
            core::input_action::find_by_name("list_box_select"),
            [this](const event_data_t& data) {
                _selection = _row + _selected_item;
                if (_on_clicked) {
                    _on_clicked();
                    return true;
                }
                return false;
            });
    }

    void list_box::on_initialize() {
        tab_stop(true);
        define_actions();
        bind_events();
    }

    border::types list_box::border() const {
        return _border;
    }

    void list_box::calculate_visible_rows() {
        auto row_height = font_face()->line_height + 2;
        _visible_rows = static_cast<uint32_t>((client_bounds().height() / row_height) - 2);
        if (_visible_rows > _options.size())
            _visible_rows = static_cast<int>(_options.size());
    }

    void list_box::border(border::types value) {
        _border = value;
    }

    void list_box::on_draw(core::renderer& surface) {
        surface.push_blend_mode(SDL_BLENDMODE_BLEND);

        auto bounds = client_bounds();

        auto pal = *palette();
        auto fg = pal[fg_color()];
        auto bg = pal[bg_color()];

        if (!enabled()) {
            fg = fg - 45;
            bg = bg - 45;
        } else if (!focused()) {
            fg = fg - 35;
            bg = bg - 35;
        }

        surface.set_color(bg);
        surface.fill_rect(bounds);

        if (_border == border::types::solid) {
            surface.set_color(fg);
            surface.draw_rect(bounds);
        }

        auto y = bounds.top() + 4;
        auto start = _row;
        auto stop = start + _visible_rows;
        if (stop > _options.size())
            stop = static_cast<int>(_options.size());
        for (auto row = start; row < stop; ++row) {
            core::rect line = {
                bounds.left() + 3,
                y,
                bounds.width() - 8,
                font_face()->line_height};
            if (row == _selection) {
                surface.draw_selection_rect(line, pal[_selection_color]);
            } else if (row == _row + _selected_item) {
                surface.draw_selection_rect(line, pal[_row_color]);
            }
            surface.draw_text_aligned(
                font_face(),
                _options[row].text,
                line,
                alignment::horizontal::left,
                alignment::vertical::middle);
            y += font_face()->line_height + 1;
        }

        surface.pop_blend_mode();
    }

    void list_box::row_color(palette_index value) {
        _row_color = value;
    }

    void list_box::selection_color(palette_index value) {
        _selection_color = value;
    }

    void list_box::on_resize(const core::rect& context_bounds) {
        switch (sizing()) {
            case sizing::parent: {
                auto container = parent();
                bounds(container != nullptr ? container->bounds() : context_bounds);
                break;
            }
            default: {
                break;
            }
        }
        calculate_visible_rows();
    }

    void list_box::on_clicked(const list_box::on_clicked_callable& callable) {
        _on_clicked = callable;
    }

}