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
            core::view_host* host) : core::view(types::control, name, host),
                                     _caret("pick-list-caret", host) {
    }

    pick_list::~pick_list() {
    }

    bool pick_list::page_up() {
        if (_visible_rows > _row) {
            _row = 0;
            return true;
        }
        else
            _row -= _visible_rows;
        return false;
    }

    bool pick_list::move_up() {
        _selected_item--;
        if (_selected_item < 0) {
            _selected_item = 0;
            return move_row_up();
        }
        return false;
    }

    void pick_list::move_top() {
        _row = 0;
        _selected_item = 0;
    }

    bool pick_list::move_down() {
        _selected_item++;
        if (_selected_item > _visible_rows - 1) {
            _selected_item = _visible_rows - 1;
            return move_row_down();
        }
        return false;
    }

    bool pick_list::page_down() {
        auto next_stop_row = _row + _visible_rows;
        if (next_stop_row > _options.size() - 1) {
            _row = static_cast<uint32_t>(_options.size() - _visible_rows);
            return true;
        }
        else
            _row += _visible_rows;
        return false;
    }

    int pick_list::width() const {
        return _width;
    }

    int pick_list::height() const {
        return _height;
    }

    void pick_list::define_actions() {
        auto up_action = core::input_action::create_no_map(
            "pick_list_up_action",
            "Internal",
            "Move to the previous pick list item.");
        if (!up_action->has_bindings())
            up_action->bind_keys({core::key_up});

        auto page_up_action = core::input_action::create_no_map(
            "pick_list_page_up_action",
            "Internal",
            "Move to the previous item page.");
        if (!page_up_action->has_bindings())
            page_up_action->bind_keys({core::key_page_up});

        auto down_action = core::input_action::create_no_map(
            "pick_list_down_action",
            "Internal",
            "Move to the next pick list item.");
        if (!down_action->has_bindings())
            down_action->bind_keys({core::key_down});

        auto page_down_action = core::input_action::create_no_map(
            "pick_list_page_down_action",
            "Internal",
            "Move to the next item page.");
        if (!page_down_action->has_bindings())
            page_down_action->bind_keys({core::key_page_down});

        auto select_action = core::input_action::create_no_map(
            "pick_list_select_action",
            "Internal",
            "Make current item the selected value.");
        if (!select_action->has_bindings())
            select_action->bind_keys({core::key_return});

        auto backspace_action = core::input_action::create_no_map(
            "pick_list_backspace_action",
            "Internal",
            "Delete a character from the type ahead search.");
        if (!backspace_action->has_bindings()) {
            backspace_action->bind_keys({core::key_backspace});
        }

        auto delete_action = core::input_action::create_no_map(
            "pick_list_delete_action",
            "Internal",
            "Reset the type ahead search.");
        if (!delete_action->has_bindings()) {
            delete_action->bind_keys({core::key_delete});
        }

        auto text_input_action = core::input_action::create_no_map(
            "pick_list_text_input",
            "Internal",
            "Any ASCII text input (non-mappable).");
        if (!text_input_action->has_bindings()) {
            text_input_action->bind_text_input();
        }
    }

    void pick_list::bind_events() {
        action_provider().register_handler(
            core::input_action::find_by_name("pick_list_up_action"),
            [this](const event_data_t& data) {
                move_up();
                return true;
            });
        action_provider().register_handler(
            core::input_action::find_by_name("pick_list_page_up_action"),
            [this](const event_data_t& data) {
                page_up();
                return true;
            });
        action_provider().register_handler(
            core::input_action::find_by_name("pick_list_down_action"),
            [this](const event_data_t& data) {
                move_down();
                return true;
            });
        action_provider().register_handler(
            core::input_action::find_by_name("pick_list_page_down_action"),
            [this](const event_data_t& data) {
                page_down();
                return true;
            });
        action_provider().register_handler(
            core::input_action::find_by_name("pick_list_select_action"),
            [this](const event_data_t& data) {
                _search.clear();
                _caret.column(0);
                _selection = _row + _selected_item;
                value(_options[_selection].text);
                return true;
            });
        action_provider().register_handler(
            core::input_action::find_by_name("pick_list_backspace_action"),
            [this](const event_data_t& data) {
                move_top();
                _search = _search.substr(0, _search.length() - 1);
                _caret.column(static_cast<uint8_t>(_search.length()));
                _found = find_matching_text(_search);
                return true;
            });
        action_provider().register_handler(
            core::input_action::find_by_name("pick_list_delete_action"),
            [this](const event_data_t& data) {
                reset_search();
                return true;
            });
        action_provider().register_handler(
            core::input_action::find_by_name("pick_list_text_input"),
            [this](const core::event_data_t& data) {
                if (data.c == core::ascii_tab
                ||  data.c == core::ascii_return
                ||  data.c == core::ascii_escape) {
                    return false;
                }

                if (_search.length() < 32) {
                    _search += data.c;
                    _caret.column(static_cast<uint8_t>(_search.length()));
                    _found = find_matching_text(_search);
                }

                return true;
            });
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

    void pick_list::reset_search() {
        move_top();
        _search.clear();
        _caret.column(0);
        _found = false;
    }

    void pick_list::on_initialize() {
        define_actions();
        bind_events();

        _caret.index(1001);
        _caret.enabled(false);
        _caret.position(0, 0);
        _caret.page_size(1, 128);
        _caret.palette(palette());
        _caret.fg_color(fg_color());
        _caret.bg_color(bg_color());
        _caret.font_family(font_family());
        _caret.initialize();

        add_child(&_caret);

        tab_stop(true);
    }

    bool pick_list::move_row_down() {
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

    void pick_list::width(int value) {
        _width = value;
    }

    void pick_list::height(int value) {
        _height = value;
    }

    void pick_list::length(int value) {
        _length = value;
    }

    option_list& pick_list::options() {
        return _options;
    }

    void pick_list::on_focus_changed() {
        index(static_cast<uint16_t>(focused() ? 1000 : 0));
    }

    std::string pick_list::value() const {
        return view::value();
    }

    void pick_list::on_palette_changed() {
        _caret.palette(palette());
    }

    void pick_list::update_minimum_size() {
        auto& minimum_size = min_size();
        minimum_size.dimensions(
            static_cast<uint32_t>(font_face()->measure_chars(_length)),
            static_cast<uint32_t>(font_face()->line_height));
    }

    uint32_t pick_list::visible_rows() const {
        return _visible_rows;
    }

    void pick_list::on_font_family_changed() {
        update_minimum_size();
    }

    void pick_list::selected_key(uint32_t key) {
        auto it = std::find_if(
            _options.begin(),
            _options.end(),
            [&key](const pick_list_option_t& o) { return o.key == key; });
        if (it != _options.end()) {
            view::value((*it).text);
            _selection = static_cast<int>(std::distance(_options.begin(), it));
            find_matching_text(value());
        }
    }

    void pick_list::visible_rows(uint32_t value) {
        if (_visible_rows != value) {
            _visible_rows = value;
        }
    }

    void pick_list::value(const std::string& text) {
        view::value(text);

        if (!text.empty()) {
            auto it = std::find_if(
                _options.begin(),
                _options.end(),
                [&text](const pick_list_option_t& o) { return o.text == text; });
            if (it != _options.end()) {
                _selection = static_cast<int>(std::distance(_options.begin(), it));
                find_matching_text(value());
            }
        }
    }

    void pick_list::row_color(palette_index value) {
        _row_color = value;
    }

    void pick_list::on_draw(core::renderer& surface) {
        auto inner_rect = inner_bounds();

        auto pal = *palette();
        auto fg = pal[fg_color()];
        auto bg = pal[bg_color()];

        if (!enabled() || !focused()) {
            fg = fg - 35;
        }

        surface.set_color(bg);
        surface.fill_rect(inner_rect);

        surface.set_font_color(font_face(), fg);
        surface.set_color(fg);

        if (_search.empty()) {
            _caret.enabled(false);
            surface.draw_text_aligned(
                font_face(),
                value(),
                inner_rect,
                alignment::horizontal::left,
                alignment::vertical::middle);
        } else {
            _caret.enabled(focused());
            if (!_found)
                surface.set_font_color(font_face(), pal[_not_found_color]);
            surface.draw_text_aligned(
                font_face(),
                _search,
                inner_rect,
                alignment::horizontal::left,
                alignment::vertical::middle);
            surface.set_font_color(font_face(), fg);
        }

        surface.draw_line(
            inner_rect.left(),
            inner_rect.bottom() - 1,
            inner_rect.right(),
            inner_rect.bottom() - 1);

        if (!focused())
            return;

        int32_t height = _height > 0 ?
            _height :
            font_face()->line_height * (_visible_rows + 1);
        auto width = _width > 0 ? _width : inner_rect.width();
        core::rect box {inner_rect.left(), inner_rect.bottom() - 1, width + 6, height};

        surface.push_clip_rect(box);

        surface.set_color(bg);
        surface.fill_rect(box);
        surface.set_color(fg);
        surface.draw_rect(box);

        auto y = box.top() + 4;
        auto start = _row;
        uint32_t stop = start + _visible_rows;
        if (stop > _options.size())
            stop = static_cast<int>(_options.size());
        for (auto row = start; row < stop; ++row) {
            core::rect line = {
                inner_rect.left() + 4,
                y,
                inner_rect.width() - 2,
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

        surface.pop_clip_rect();
    }

    void pick_list::selection_color(palette_index value) {
        _selection_color = value;
    }

    void pick_list::not_found_color(palette_index value) {
        _not_found_color = value;
    }

    bool pick_list::find_matching_text(const std::string& text) {
        move_top();
        auto text_length = text.length();
        for (const auto& option : _options) {
            if (option.text.substr(0, text_length) == text) {
                return true;
            }
            move_down();
        }
        return false;
    }

}