//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <core/views/label.h>
#include <core/views/button.h>
#include <core/views/console.h>
#include <core/views/textbox.h>
#include <core/views/notebook.h>
#include <core/views/checkbox.h>
#include <core/views/pick_list.h>
#include <core/views/text_editor.h>
#include <core/views/memory_editor.h>
#include <core/views/dock_layout_panel.h>
#include "state.h"
#include "view_factory.h"

namespace ryu::core {

    void view_factory::configure_view(
            core::view* view,
            core::font_family* font_family,
            core::palette* palette,
            uint8_t fg_color,
            uint8_t bg_color,
            dock::styles dock_style,
            const rect& bounds,
            const padding& margin,
            const padding& padding) {
        view->font_family(font_family);
        view->palette(palette);
        view->dock(dock_style);
        view->fg_color(fg_color);
        view->bg_color(bg_color);
        view->margin(margin);
        view->padding(padding);
        view->bounds(bounds);
        view->initialize();
    }

    pick_list_unique_ptr view_factory::create_pick_list(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            const option_list& options,
            core::dock::styles dock_style,
            const core::padding& margin,
            const core::padding& padding,
            const core::rect& bounds) {
        auto host = dynamic_cast<view_host*>(state);

        auto view = std::make_unique<core::pick_list>(name, host);
        configure_view(
            view.get(),
            state->context()->font_family(),
            &state->context()->palette(),
            fg_color,
            bg_color,
            dock_style,
            bounds,
            margin,
            padding);

        auto& list_options = view->options();
        for (const auto& option : options)
            list_options.push_back(option);

        return view;
    }

    label_unique_ptr view_factory::create_label(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            const std::string& value,
            core::dock::styles dock_style,
            const core::padding& margin,
            const core::padding& padding,
            const core::rect& bounds) {
        auto host = dynamic_cast<view_host*>(state);

        auto view = std::make_unique<core::label>(name, host);
        configure_view(
            view.get(),
            state->context()->font_family(),
            &state->context()->palette(),
            fg_color,
            bg_color,
            dock_style,
            bounds,
            margin,
            padding);
        view->value(value);
        return view;
    }

    button_unique_ptr view_factory::create_button(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            const std::string& value,
            core::dock::styles dock_style,
            const core::padding& margin,
            const core::padding& padding,
            const core::rect& bounds) {
        auto host = dynamic_cast<view_host*>(state);

        auto view = std::make_unique<core::button>(name, host);
        configure_view(
            view.get(),
            state->context()->font_family(),
            &state->context()->palette(),
            fg_color,
            bg_color,
            dock_style,
            bounds,
            margin,
            padding);
        view->value(value);
        return view;
    }

    checkbox_unique_ptr view_factory::create_checkbox(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            bool value,
            core::dock::styles dock_style,
            const core::padding& margin,
            const core::padding& padding,
            const core::rect& bounds) {
        auto host = dynamic_cast<view_host*>(state);

        auto view = std::make_unique<core::checkbox>(name, host);
        configure_view(
            view.get(),
            state->context()->font_family(),
            &state->context()->palette(),
            fg_color,
            bg_color,
            dock_style,
            bounds,
            margin,
            padding);
        view->value(value);
        return view;
    }

    console_unique_ptr view_factory::create_console(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            const core::code_to_attr_dict& mapper,
            core::dock::styles dock_style,
            const core::padding& margin,
            const core::padding& padding,
            const core::rect& bounds) {
        auto host = dynamic_cast<view_host*>(state);

        auto view = std::make_unique<core::console>(name, host);
        configure_view(
            view.get(),
            state->context()->font_family(),
            &state->context()->palette(),
            fg_color,
            bg_color,
            dock_style,
            bounds,
            margin,
            padding);
        view->code_mapper(mapper);
        return view;
    }

    dock_layout_panel_unique_ptr view_factory::create_dock_layout_panel(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            dock::styles dock_style,
            const core::padding& margin,
            const core::padding& padding) {
        auto host = dynamic_cast<view_host*>(state);

        auto view = std::make_unique<core::dock_layout_panel>(name, host);
        configure_view(
            view.get(),
            state->context()->font_family(),
            &state->context()->palette(),
            fg_color,
            bg_color,
            dock_style,
            {},
            margin,
            padding);
        return view;
    }

    notebook_unique_ptr view_factory::create_notebook(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            dock::styles dock_style,
            const padding& margin,
            const padding& padding,
            const rect& bounds) {
        auto host = dynamic_cast<view_host*>(state);

        auto view = std::make_unique<core::notebook>(name, host);
        configure_view(
            view.get(),
            state->context()->font_family(),
            &state->context()->palette(),
            fg_color,
            bg_color,
            dock_style,
            bounds,
            margin,
            padding);
        return view;
    }

    textbox_unique_ptr view_factory::create_textbox(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            const std::string& value,
            dock::styles dock_style,
            const padding& margin,
            const padding& padding,
            const rect& bounds) {
        auto host = dynamic_cast<view_host*>(state);

        auto view = std::make_unique<core::textbox>(name, host);
        configure_view(
            view.get(),
            state->context()->font_family(),
            &state->context()->palette(),
            fg_color,
            bg_color,
            dock_style,
            bounds,
            margin,
            padding);
        view->value(value);
        return view;
    }

    memory_editor_unique_ptr view_factory::create_memory_editor(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            dock::styles dock_style,
            const padding& margin,
            const padding& padding,
            const rect& bounds) {
        auto host = dynamic_cast<view_host*>(state);

        auto view = std::make_unique<core::memory_editor>(name, host);
        configure_view(
            view.get(),
            state->context()->font_family(),
            &state->context()->palette(),
            fg_color,
            bg_color,
            dock_style,
            bounds,
            margin,
            padding);
        return view;
    }

    text_editor_unique_ptr view_factory::create_text_editor(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            uint32_t rows,
            uint16_t columns,
            dock::styles dock_style,
            const padding& margin,
            const padding& padding,
            const rect& bounds) {
        auto host = dynamic_cast<view_host*>(state);

        auto view = std::make_unique<core::text_editor>(name, host);
        configure_view(
            view.get(),
            state->context()->font_family(),
            &state->context()->palette(),
            fg_color,
            bg_color,
            dock_style,
            bounds,
            margin,
            padding);
        view->size(rows, columns);
        return view;
    }

}