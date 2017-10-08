//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "notebook.h"

namespace ryu::core {

    notebook::notebook(const std::string& name) : core::view(types::control, name) {
        margin({width, 0, 0, 0});
    }

    int notebook::active_tab() const {
        return _index;
    }

    void notebook::active_tab(int index) {
        _index = index;
    }

    void notebook::remove_tab(int index) {
        _tabs.erase(_tabs.begin() + index);
    }

    void notebook::on_draw(core::renderer& surface) {
        auto rect = bounds();

        auto pal = *palette();
        auto& fg = pal[fg_color()];
        auto& bg = pal[bg_color()];

        if (!enabled() || !focused())
            fg = fg - 35;

        surface.set_color(fg);

        auto& margins = margin();
        core::rect content_rect {width,
                                 rect.top() + margins.top(),
                                 rect.right() - margins.right() - width,
                                 rect.bottom() - margins.bottom()};
        size_t index = 0;
        auto y = rect.top();
        for (const auto& tab : _tabs) {
            core::rect tab_rect {0, y, width, 50};

            if (index == _index) {
                surface.set_font_color(font_face(), bg);
                surface.fill_rect(tab_rect);
            } else {
                surface.set_font_color(font_face(), fg);
                surface.draw_rect(tab_rect);
            }

            auto child = get_child_at(index);
            if (child != nullptr)
                child->visible(index == _index);

            surface.draw_text_aligned(
                    font_face(),
                    tab,
                    tab_rect,
                    alignment::horizontal::center,
                    alignment::vertical::middle);

            y += 50;
            index++;
        }

        surface.draw_rect(content_rect);
    }

    bool notebook::on_process_event(const SDL_Event* e) {
        if (focused()) {
            if (e->type == SDL_KEYDOWN) {
                switch (e->key.keysym.sym) {
                    case SDLK_UP: {
                        _index--;
                        if (_index < 0)
                            _index = 0;
                        return true;
                    }
                    case SDLK_DOWN: {
                        if (_index + 1 < _tabs.size())
                            _index++;
                        return true;
                    }
                    case SDLK_SPACE: {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void notebook::on_resize(const core::rect& context_bounds) {
        core::view::on_resize(context_bounds);
    }

    void notebook::add_tab(const std::string& title, core::view* content) {
        _tabs.push_back(title);
        if (content != nullptr) {
            content->visible(false);
            add_child(content);
        }
    }

}