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
        core::view_host* host) : core::panel(name, host) {
    }

    palette_editor::~palette_editor() {
    }

    void palette_editor::bind_events() {
    }

    void palette_editor::on_initialize() {
        define_actions();
        bind_events();

        tab_stop(true);
        should_clip(true);

        auto& minimum_size = min_size();
        minimum_size.dimensions(256, 256);
    }

    void palette_editor::define_actions() {

    }

    void palette_editor::rebuild_entries() {
        clear_children();
        _views.clear();

        if (palette() == nullptr)
            return;

        for (size_t entry_index = 0;
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
            entry_editor->entry(static_cast<core::palette_index>(entry_index));

            add_child(entry_editor.get());

            _views.push_back(std::move(entry_editor));
        }

        for (size_t i = 0; i < _views.size(); i++) {
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
    }

    void palette_editor::on_focus_changed() {
        border(focused() ? border::types::solid : border::types::none);
    }

    void palette_editor::on_palette_changed() {
        if (_views.empty()) {
            rebuild_entries();
        } else {
            for (const auto& view : _views)
                view->palette(palette());
        }
    }

    void palette_editor::on_font_family_changed() {
        for (const auto& view : _views)
            view->font_family(font_family());
    }

}