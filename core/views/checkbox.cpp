//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <core/input_binding.h>
#include <core/input_action.h>
#include "checkbox.h"

namespace ryu::core {

    checkbox::checkbox(
            const std::string& name,
            core::view_host* host) : core::view(types::control, name, host) {
    }

    checkbox::~checkbox() {
    }

    void checkbox::define_actions() {
        auto activate_action = core::input_action::create_no_map(
                "checkbox_activate",
                "Internal",
                "Activate a checkbox view.");
        if (!activate_action->has_bindings())
            activate_action->bind_keys({core::key_space});
    }

    void checkbox::bind_events() {
        action_provider().register_handler(
                core::input_action::find_by_name("checkbox_activate"),
                [this](const event_data_t& data) {
                    if (value() == "true")
                        value("false");
                    else
                        value("true");
                    return true;
                });
    }

    void checkbox::on_initialize() {
        tab_stop(true);
        define_actions();
        bind_events();
        value("false");
    }

    void checkbox::on_draw(core::renderer& surface) {
        auto bounds = client_bounds();

        auto pal = *palette();
        auto& fg = pal[fg_color()];

        if (!enabled() || !focused()) {
            fg = fg - 35;
        }

        surface.set_color(fg);
        surface.draw_rect(bounds);

        if (value() == "true") {
            surface.draw_line(bounds.left(), bounds.top(), bounds.right(), bounds.bottom());
        }
    }

}