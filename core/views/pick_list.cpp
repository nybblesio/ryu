//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "pick_list.h"

namespace ryu::core {

    pick_list::pick_list(const std::string& name) : core::view(types::control, name) {
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
        auto max = std::min(_visibile_items, static_cast<const int&>(_options.size())) - 1;
        if (_selection > max) {
            _selection = max;
            return move_row_down();
        }
        return false;
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

    void pick_list::on_draw(SDL_Renderer* renderer) {
        auto bounds = client_bounds();

        auto pal = *palette();
        auto fg = pal[fg_color()];
        auto& bg = pal[bg_color()];

        if (!enabled() || !focused()) {
            fg = fg - 35;
        }

        set_color(renderer, bg);
        fill_rect(renderer, bounds);

        set_font_color(fg);
        set_color(renderer, fg);

        draw_text_aligned(renderer,
                          _value,
                          bounds,
                          alignment::horizontal::left,
                          alignment::vertical::middle);
        if (_border == border::types::solid) {
            set_color(renderer, fg);
            draw_rect(renderer, bounds);
        } else {
            draw_line(renderer, bounds.left(), bounds.bottom(), bounds.right() + 5, bounds.bottom());
        }

        if (focused()) {
            core::rect box {bounds.left(),
                            bounds.bottom(),
                            bounds.width() + 6,
                            (font_face()->line_height * (_visibile_items + 1))};
            set_color(renderer, fg);
            draw_rect(renderer, box);

            auto y = box.top() + 4;
            auto start = _row;
            auto max = std::min(_visibile_items, static_cast<const int&>(_options.size()));
            auto stop = _row + max;
            for (auto row = start; row < stop; ++row) {
                core::rect line = {bounds.left() + 4, y, bounds.width() - 2, font_face()->line_height};
                if (row == _row + _selection) {
                    push_blend_mode(renderer, SDL_BLENDMODE_BLEND);
                    auto selection_color = pal[fg_color()];
                    selection_color.alpha(0x5f);
                    set_color(renderer, selection_color);
                    fill_rect(renderer, line);
                    pop_blend_mode(renderer);
                }
                draw_text_aligned(renderer,
                                  _options[row],
                                  line,
                                  alignment::horizontal::left,
                                  alignment::vertical::middle);
                y += font_face()->line_height + 1;
            }
        }
    }

    bool pick_list::on_process_event(const SDL_Event* e) {
        if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_UP: {
                    move_up();
                    return true;
                }
                case SDLK_DOWN: {
                    move_down();
                    return true;
                }
                case SDLK_RETURN: {
                    _value = _options[_row + _selection];
                    return true;
                }
            }
        }
        return false;
    }

    void pick_list::on_resize(const core::rect& context_bounds) {
        bounds().size(font_face()->width * (_length + 1), font_face()->line_height + 10);
    }

}