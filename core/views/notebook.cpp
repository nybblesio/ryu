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
        const auto width = 150;

        auto bounds = client_bounds();

        auto pal = *palette();
        auto& fg = pal[fg_color()];
        auto& bg = pal[bg_color()];

//        if (!enabled() || !focused())
//            fg = fg - 35;

        surface.set_color(fg);

        auto& margins = margin();
        core::rect content_rect {width,
                                 bounds.top() + margins.top(),
                                 bounds.right() - margins.right() - width,
                                 bounds.bottom() - margins.bottom()};
        auto index = 0;
        auto y = bounds.top();
        for (const auto& tab : _tabs) {
            core::rect tab_rect {0, y, width, 50};
            if (index == _index) {
                surface.set_font_color(font_face(), bg);
                surface.fill_rect(tab_rect);
                if (tab.content != nullptr) {
                    surface.push_clip_rect(content_rect);
                    tab.content->draw(surface);
                    surface.pop_clip_rect();
                }
            } else {
                surface.set_font_color(font_face(), fg);
                surface.draw_rect(tab_rect);
            }
            surface.draw_text_aligned(
                    font_face(),
                    tab.title,
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
        if (!_tabs.empty()) {
            const auto& tab = _tabs[_index];
            if (tab.content != nullptr)
                return tab.content->process_event(e);
        }
        return false;
    }

    void notebook::on_resize(const core::rect& context_bounds) {
//        switch (sizing()) {
//            case sizing::parent: {
//                auto container = parent();
//                bounds(container != nullptr ? container->bounds() : context_bounds);
//                break;
//            }
//            default: {
//                break;
//            }
//        }
        if (!_tabs.empty()) {
            const auto& tab = _tabs[_index];
            if (tab.content != nullptr)
                return tab.content->requires_layout();
        }
    }

    void notebook::add_tab(const std::string& title, core::view* content) {
        _tabs.push_back(tab_t {title, content});
    }

}