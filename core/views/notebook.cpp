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
#include "notebook.h"

namespace ryu::core {

    notebook::notebook(
            const std::string& name,
            core::view_host* host) : core::view(types::control, name, host) {
    }

    notebook::~notebook() {
    }

    void notebook::define_actions() {
        auto prev_tab_action = core::input_action::create_no_map(
                "notebook_previous_tab",
                "Internal",
                "Move to the previous notebook tab.");
        prev_tab_action->bind_keys({core::key_up});

        auto next_tab_action = core::input_action::create_no_map(
                "notebook_next_tab",
                "Internal",
                "Move to the next notebook tab.");
        next_tab_action->bind_keys({core::key_down});
    }

    void notebook::bind_events() {
//        prev_tab_action->register_handler(
//                action_sink::types::view,
//                [this](const event_data_t& data) {
//                    return focused();
//                },
//                [this](const event_data_t& data) {
//                    _index--;
//                    if (_index < 0)
//                        _index = 0;
//                    return true;
//                });
//        next_tab_action->register_handler(
//                action_sink::types::view,
//                [this](const event_data_t& data) {
//                    return focused();
//                },
//                [this](const event_data_t& data) {
//                    if (_index + 1 < _tabs.size())
//                        _index++;
//                    return true;
//                });
    }

    void notebook::on_initialize() {
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

    core::rect notebook::client_bounds() {
        auto& rect = bounds();
        auto& margins = margin();
        return {rect.left() + (tab_width + 1),
                rect.top() + 1,
                rect.width() - ((tab_width + 1) + margins.left() + margins.right()),
                rect.height() - 2};
    }

    void notebook::on_draw(core::renderer& surface) {
        auto& rect = bounds();
        auto& pads = padding();

        auto pal = *palette();
        auto& fg = pal[fg_color()];
        auto& bg = pal[bg_color()];

        if (!enabled() || !focused())
            fg = fg - 35;

        surface.set_color(fg);

        size_t index = 0;
        auto y = rect.top() + pads.top();
        auto x = rect.left() + pads.left();
        for (const auto& tab : _tabs) {
            core::rect tab_rect {x, y, tab_width, tab_height};

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

            y += (tab_height - 1);
            index++;
        }

        auto content_rect = bounds();
        content_rect.pos(rect.left() + (tab_width - 1), rect.top())
                    .size(rect.width() - (tab_width + 1), rect.height());
        surface.draw_rect(content_rect);
    }

    void notebook::draw_children(core::renderer& surface) {
        surface.push_clip_rect(client_bounds());
        view::draw_children(surface);
        surface.pop_clip_rect();
    }

    void notebook::on_resize(const core::rect& context_bounds) {
        core::view::on_resize(context_bounds);

        auto rect = bounds();
        bounds({rect.left(),
                rect.top(),
                rect.width(),
                rect.height()});
    }

    void notebook::add_tab(const std::string& title, core::view* content) {
        _tabs.push_back(title);
        if (content != nullptr) {
            content->visible(false);
            add_child(content);
        }
    }

}