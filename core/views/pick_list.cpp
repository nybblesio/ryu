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
#include "pick_list.h"

namespace ryu::core {

    pick_list::pick_list(
            const std::string& name,
            core::view_host* host) : core::view(types::control, name, host) {
    }

    pick_list::~pick_list() {
    }

    bool pick_list::move_up() {
        _selection--;
        if (_selection < 0) {
            _selection = 0;
            return move_row_up();
        }
        return false;
    }

    bool pick_list::move_down() {
        _selection++;
        auto max = std::min(
            _visibile_items,
            static_cast<const int&>(_options.size())) - 1;
        if (_selection > max) {
            _selection = max;
            return move_row_down();
        }
        return false;
    }

    void pick_list::define_actions() {
        auto up_action = core::input_action::create_no_map(
                "pick_list_up_action",
                "Internal",
                "Move to the previous pick list item.");
        up_action->bind_keys({core::key_up});

        auto down_action = core::input_action::create_no_map(
                "pick_list_down_action",
                "Internal",
                "Move to the next pick list item.");
        down_action->bind_keys({core::key_down});

        auto select_action = core::input_action::create_no_map(
                "pick_list_select_action",
                "Internal",
                "Make current item the selected value.");
        select_action->bind_keys({core::key_return});
    }

    void pick_list::bind_events() {
//        up_action->register_handler(
//            action_sink::types::view,
//            [this](const event_data_t& data) {
//                return focused();
//            },
//            [this](const event_data_t& data) {
//                move_up();
//                return true;
//            });
//        down_action->register_handler(
//            action_sink::types::view,
//            [this](const event_data_t& data) {
//                return focused();
//            },
//            [this](const event_data_t& data) {
//                move_down();
//                return true;
//            });
//        select_action->register_handler(
//            action_sink::types::view,
//            [this](const event_data_t& data) {
//                return focused();
//            },
//            [this](const event_data_t& data) {
//                _value = _options[_row + _selection];
//                return true;
//            });
    }

    bool pick_list::move_row_up() {
        _row--;
        if (_row < 0) {
            _row = 0;
            return true;
        }
        return false;
    }

    int pick_list::length() const {
        return _length;
    }

    std::string pick_list::value() {
        return _value;
    }

    void pick_list::on_initialize() {
        tab_stop(true);
        bind_events();
        padding({5, 5, 5, 5});
    }

    bool pick_list::move_row_down() {
        if (_options.size() < _visibile_items)
            return false;
        _row++;
        auto max = _options.size() - _visibile_items;
        if (_row > max) {
            _row = static_cast<int>(max);
            return true;
        }
        return false;
    }

    void pick_list::length(int value) {
        _length = value;
        requires_layout();
    }

    option_list& pick_list::options() {
        return _options;
    }

    int pick_list::visible_items() const {
        return _visibile_items;
    }

    border::types pick_list::border() const {
        return _border;
    }

    void pick_list::visible_items(int value) {
        _visibile_items = value;
    }

    void pick_list::border(border::types value) {
        _border = value;
    }

    void pick_list::value(const std::string& value) {
        _value = value;
    }

    void pick_list::on_draw(core::renderer& surface) {
        auto bounds = client_bounds();

        auto pal = *palette();
        auto fg = pal[fg_color()];
        auto& bg = pal[bg_color()];

        if (!enabled() || !focused()) {
            fg = fg - 35;
        }

        surface.set_color(bg);
        surface.fill_rect(bounds);

        surface.set_font_color(font_face(), fg);
        surface.set_color(fg);

        surface.draw_text_aligned(
            font_face(),
            _value,
            bounds,
            alignment::horizontal::left,
            alignment::vertical::middle);
        if (_border == border::types::solid) {
            surface.set_color(fg);
            surface.draw_rect(bounds);
        } else {
            surface.draw_line(
                bounds.left(),
                bounds.bottom(),
                bounds.right() + 5,
                bounds.bottom());
        }

        if (focused()) {
            core::rect box {
                bounds.left(),
                bounds.bottom(),
                bounds.width() + 6,
                (font_face()->line_height * (_visibile_items + 1))};
            surface.set_color(fg);
            surface.draw_rect(box);

            auto y = box.top() + 4;
            auto start = _row;
            auto max = std::min(
                _visibile_items,
                static_cast<int32_t>(_options.size()));
            auto stop = _row + max;
            for (auto row = start; row < stop; ++row) {
                core::rect line = {
                    bounds.left() + 4,
                    y,
                    bounds.width() - 2,
                    font_face()->line_height};
                if (row == _row + _selection) {
                    surface.push_blend_mode(SDL_BLENDMODE_BLEND);
                    auto selection_color = pal[fg_color()];
                    selection_color.alpha(0x5f);
                    surface.set_color(selection_color);
                    surface.fill_rect(line);
                    surface.pop_blend_mode();
                }
                surface.draw_text_aligned(
                    font_face(),
                    _options[row],
                    line,
                    alignment::horizontal::left,
                    alignment::vertical::middle);
                y += font_face()->line_height + 1;
            }
        }
    }

    void pick_list::on_resize(const core::rect& context_bounds) {
        bounds().size(
            font_face()->width * (_length + 1),
            font_face()->line_height + 10);
    }

}