//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#pragma once

#include <memory>
#include "view.h"
#include "core_types.h"

namespace ryu::core {

    class button;
    using button_unique_ptr = std::unique_ptr<core::button>;

    class caret;
    using caret_unique_ptr = std::unique_ptr<core::caret>;

    class state_header;
    using state_header_unique_ptr = std::unique_ptr<core::state_header>;

    class document_footer;
    using document_footer_unique_ptr = std::unique_ptr<core::document_footer>;

    class memory_editor;
    using memory_editor_unique_ptr = std::unique_ptr<core::memory_editor>;

    class console;
    using console_unique_ptr = std::unique_ptr<core::console>;

    class checkbox;
    using checkbox_unique_ptr = std::unique_ptr<core::checkbox>;

    class dock_layout_panel;
    using dock_layout_panel_unique_ptr = std::unique_ptr<core::dock_layout_panel>;

    class label;
    using label_unique_ptr = std::unique_ptr<core::label>;

    class memory_editor;
    using memory_editor_unique_ptr = std::unique_ptr<core::memory_editor>;

    class notebook;
    using notebook_unique_ptr = std::unique_ptr<core::notebook>;

    class panel;
    using panel_unique_ptr = std::unique_ptr<core::panel>;

    class pick_list;
    using pick_list_unique_ptr = std::unique_ptr<core::pick_list>;

    class text_editor;
    using text_editor_unique_ptr = std::unique_ptr<core::text_editor>;

    class textbox;
    using textbox_unique_ptr = std::unique_ptr<core::textbox>;

    class column_pick_list;
    using column_pick_list_unique_ptr = std::unique_ptr<core::column_pick_list>;

    class view_factory {
    public:
        static caret_unique_ptr create_caret(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            const core::rect& bounds = {});

        static column_pick_list_unique_ptr create_column_pick_list(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            const padding& margin = {},
            const padding& padding = {});

        static label_unique_ptr create_label(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            const std::string& value = "",
            core::dock::styles dock_style = dock::styles::left,
            const core::padding& margin = {},
            const core::padding& padding = {},
            const core::rect& bounds = {});

        static button_unique_ptr create_button(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            const std::string& value = "",
            core::dock::styles dock_style = dock::styles::left,
            const core::padding& margin = {},
            const core::padding& padding = {},
            const core::rect& bounds = {});

        static textbox_unique_ptr create_textbox(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            const std::string& value = "",
            core::dock::styles dock_style = dock::styles::left,
            const core::padding& margin = {},
            const core::padding& padding = {},
            const core::rect& bounds = {});

        static notebook_unique_ptr create_notebook(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            core::dock::styles dock_style = dock::styles::fill,
            const core::padding& margin = {},
            const core::padding& padding = {},
            const core::rect& bounds = {});

        static checkbox_unique_ptr create_checkbox(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            bool value = false,
            dock::styles dock_style = dock::styles::left,
            const padding& margin = {},
            const padding& padding = {},
            const core::rect& bounds = {});

        static pick_list_unique_ptr create_pick_list(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            const option_list& options = {},
            dock::styles dock_style = dock::styles::left,
            const padding& margin = {},
            const padding& padding = {},
            const core::rect& bounds = {});

        static state_header_unique_ptr create_state_header(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            dock::styles dock_style = dock::styles::fill,
            const padding& margin = {},
            const padding& padding = {});

        static document_footer_unique_ptr create_document_footer(
                core::state* state,
                const std::string& name,
                uint8_t fg_color,
                uint8_t bg_color,
                dock::styles dock_style = dock::styles::fill,
                const padding& margin = {},
                const padding& padding = {});

        static dock_layout_panel_unique_ptr create_dock_layout_panel(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            dock::styles dock_style = dock::styles::fill,
            const padding& margin = {},
            const padding& padding = {});

        static memory_editor_unique_ptr create_memory_editor(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            dock::styles dock_style = dock::styles::fill,
            const padding& margin = {},
            const padding& padding = {},
            const core::rect& bounds = {});

        static console_unique_ptr create_console(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            const core::code_to_attr_dict& mapper,
            dock::styles dock_style = dock::styles::fill,
            const padding& margin = {},
            const padding& padding = {},
            const core::rect& bounds = {});

        static text_editor_unique_ptr create_text_editor(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            uint32_t rows,
            uint16_t columns,
            dock::styles dock_style = dock::styles::fill,
            const padding& margin = {},
            const padding& padding = {},
            const core::rect& bounds = {});

    private:
        static void configure_view(
            core::view* view,
            core::font_family* font_family,
            core::palette* palette,
            uint8_t fg_color,
            uint8_t bg_color,
            core::dock::styles dock_style,
            const core::rect& bounds,
            const core::padding& margin,
            const core::padding& padding);
    };

};

