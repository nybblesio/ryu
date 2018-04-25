//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <ide/ide_types.h>
#include <ide/ide_context.h>
#include <core/input_action.h>
#include "emulator_context.h"

namespace ryu::emulator {

    emulator_context::emulator_context(const std::string& name) : core::context(name),
                                                                  _emulator_state("machine-emulator") {
    }

    void emulator_context::define_actions() {
        auto toggle_context = core::input_action::create(
                "toggle_emulator_context",
                "Emulator",
                "Collapse, expand, and split the Emulator context window.");
        if (!toggle_context->has_bindings()) {
            toggle_context->bind_keys({core::mod_alt, core::key_f2});
        }
    }

    void emulator_context::bind_events() {
        action_provider().register_handler(
            core::input_action::find_by_name("toggle_emulator_context"),
            [this](const core::event_data_t& data) {
                auto ide_context = dynamic_cast<ide::ide_context*>(engine()->find_context("ide"));
                switch (_size) {
                    case core::context_window::split:
                        size(core::context_window::expanded);
                        ide_context->size(core::context_window::collapsed);
                        break;
                    case core::context_window::expanded:
                    case core::context_window::collapsed:
                        size(core::context_window::split);
                        ide_context->size(core::context_window::split);
                        break;
                    default:
                        break;
                }
                return true;
            });
    }

    void emulator_context::configure_palette() {
        auto& pal = palette();

        auto& black = pal[ide::colors::black];
        black.red(0x00);
        black.green(0x00);
        black.blue(0x00);
        black.alpha(0xff);

        auto& dark_grey = pal[ide::colors::dark_grey];
        dark_grey.red(0x65);
        dark_grey.green(0x68);
        dark_grey.blue(0x6d);
        dark_grey.alpha(0xff);

        auto& fill_color = pal[ide::colors::fill_color];
        fill_color.red(0x65);
        fill_color.green(0x68);
        fill_color.blue(0x6d);
        fill_color.alpha(0xff);
    }

    void emulator_context::on_draw(core::renderer& surface) {
        if (size() == core::context_window::sizes::expanded)
            return;

        const int16_t tab_width = 32;
        const int16_t tab_slope = 16;
        const int16_t tab_height = 128;

        auto rect = bounds();
        auto middle = static_cast<int16_t>((rect.height() / 2) - tab_slope);
        core::vertex_list tab_vertices = {
            {static_cast<int16_t>(rect.left() - tab_width), static_cast<int16_t>(middle)},
            {static_cast<int16_t>(rect.left()),             static_cast<int16_t>(middle - tab_slope)},
            {static_cast<int16_t>(rect.left()),             static_cast<int16_t>(middle + tab_height + tab_slope)},
            {static_cast<int16_t>(rect.left() - tab_width), static_cast<int16_t>(middle + tab_height)},
            {static_cast<int16_t>(rect.left() - tab_width), static_cast<int16_t>(middle)},
        };

        auto& pal = palette();

        surface.set_color(pal[ide::colors::indexes::dark_grey]);
        surface.fill_polygon(tab_vertices);
        surface.set_color(pal[ide::colors::indexes::black]);
        surface.draw_line(
                rect.left() - tab_width,
                middle,
                rect.left(),
                middle - tab_slope);
        surface.draw_line(
                rect.left() - tab_width,
                middle + tab_height,
                rect.left() - tab_width,
                middle);
        surface.draw_line(
                rect.left() - tab_width,
                middle + tab_height,
                rect.left(),
                middle + tab_height + tab_slope);
    }

    bool emulator_context::on_initialize(core::result& result) {
        define_actions();
        bind_events();
        configure_states(result);
        configure_palette();
        parent_resize(bounds());
        return true;
    }

    core::context_window::sizes emulator_context::size() const {
        return _size;
    }

    void emulator_context::configure_states(core::result& result) {
        add_state(result, &_emulator_state);
        push_state(_emulator_state.id(), {});
    }

    void emulator_context::size(core::context_window::sizes value) {
        if (value != _size) {
            _size = value;
            engine()->raise_resize();
        }
    }

    void emulator_context::parent_resize(const core::rect& parent_bounds) {
        switch (_size) {
            case core::context_window::split:
                bounds({
                               (parent_bounds.width() / 2) + 1,
                               0,
                               (parent_bounds.width() / 2) - 1,
                               parent_bounds.height()
                       });
                break;
            case core::context_window::expanded:
                bounds({
                               16,
                               0,
                               parent_bounds.width(),
                               parent_bounds.height()
                       });
                break;
            case core::context_window::collapsed:
                bounds({
                               parent_bounds.right() - 16,
                               0,
                               parent_bounds.width(),
                               parent_bounds.height()
                       });
                break;
        }
    }

}