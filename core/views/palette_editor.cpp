//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <core/view_factory.h>
#include "palette_editor.h"
#include "palette_entry_editor.h"

namespace ryu::core {

    palette_editor::palette_editor(
        const std::string& name,
        core::view_host* host) : core::dock_layout_panel(name, host) {
    }

    palette_editor::~palette_editor() {
    }

    void palette_editor::bind_events() {

    }

    void palette_editor::on_initialize() {
        tab_stop(true);
        define_actions();
        bind_events();
    }

    void palette_editor::define_actions() {

    }

    void palette_editor::rebuild_entries() {
        clear_children();
        _views.clear();

        if (palette() == nullptr)
            return;

        for (auto entry_index = 0;
                entry_index < palette()->size();
                ++entry_index) {
            auto entry_editor = core::view_factory::create_view<core::palette_entry_editor>(
                host(),
                fmt::format("palette-entry-{}", entry_index),
                font_family(),
                palette(),
                fg_color(),
                bg_color(),
                "",
                dock::top,
                {5, 5, 5, 5},
                {},
                {});
            entry_editor->locked(true);
            entry_editor->enabled(true);
            entry_editor->tab_stop(true);
            // XXX: need to fix hard coded color
            entry_editor->text_color(15);
            entry_editor->sizing(sizing::fixed);
            entry_editor->entry(static_cast<core::palette_index>(entry_index));

            add_child(entry_editor.get());

            _views.push_back(std::move(entry_editor));
        }

        for (auto i = 0; i < _views.size(); i++) {
            if (i == 0) {
                _views[i]->next_view(_views[i + 1].get());
                _views[i]->prev_view(this);
            } else if (i == _views.size() - 1) {
                _views[i]->next_view(this);
                _views[i]->prev_view(_views[i - 1].get());
            } else {
                _views[i]->next_view(_views[i + 1].get());
                _views[i]->prev_view(_views[i - 1].get());
            }
        }

        // XXX: can't tab back to any peer views
        if (!_views.empty())
            next_view(_views[0].get());

        layout(true);
    }

    void palette_editor::on_focus_changed() {
        border(focused() ? border::types::solid : border::types::none);
    }

    void palette_editor::on_palette_changed() {
        rebuild_entries();
    }

    void palette_editor::on_draw(core::renderer& surface) {
        dock_layout_panel::on_draw(surface);
    }

}