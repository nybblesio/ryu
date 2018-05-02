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
        core::view_host* host) : core::view(core::view::types::control, name, host) {
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

        // XXX: refactor palette to support different sizes
        for (auto entry_index = 0;
                entry_index < 16;
                ++entry_index) {
            auto palette_entry_editor = core::view_factory::create_view<core::palette_entry_editor>(
                host(),
                fmt::format("palette-entry-{}", entry_index),
                font_family(),
                palette(),
                fg_color(),
                bg_color(),
                "",
                dock::top,
                {},
                {},
                {0, 0, 256, 64});
            palette_entry_editor->locked(true);
            palette_entry_editor->enabled(true);
            palette_entry_editor->sizing(sizing::fixed);
            palette_entry_editor->text_color(fg_color());
            palette_entry_editor->entry(static_cast<core::palette_index>(entry_index));

            auto dock_panel = core::view_factory::create_view<core::dock_layout_panel>(
                host(),
                fmt::format("layout-panel-{}", entry_index),
                font_family(),
                palette(),
                fg_color(),
                bg_color(),
                "",
                dock::top,
                {20, 20, 20, 20},
                {},
                {0, 0, 256, 64});
            dock_panel->add_child(palette_entry_editor.get());
            add_child(dock_panel.get());

            _views.push_back(std::move(dock_panel));
            _views.push_back(std::move(palette_entry_editor));
        }
        layout(true);
    }

    void palette_editor::on_focus_changed() {
    }

    void palette_editor::on_palette_changed() {
        rebuild_entries();
    }

    void palette_editor::on_draw(core::renderer& surface) {
    }

    void palette_editor::on_resize(const rect& context_bounds) {
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
    }

}