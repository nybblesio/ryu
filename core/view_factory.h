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
#include <core/views/label.h>
#include <core/views/panel.h>
#include <core/views/button.h>
#include <core/views/console.h>
#include <core/views/textbox.h>
#include <core/views/notebook.h>
#include <core/views/checkbox.h>
#include <core/views/pick_list.h>
#include <core/views/text_editor.h>
#include <core/views/state_header.h>
#include <core/views/memory_editor.h>
#include <core/views/loadable_view.h>
#include <core/views/document_footer.h>
#include <core/views/column_pick_list.h>
#include "core_types.h"

namespace ryu::core {

    using button_unique_ptr = std::unique_ptr<core::button>;
    using caret_unique_ptr = std::unique_ptr<core::caret>;
    using state_header_unique_ptr = std::unique_ptr<core::state_header>;
    using document_footer_unique_ptr = std::unique_ptr<core::document_footer>;
    using memory_editor_unique_ptr = std::unique_ptr<core::memory_editor>;
    using console_unique_ptr = std::unique_ptr<core::console>;
    using checkbox_unique_ptr = std::unique_ptr<core::checkbox>;
    using dock_layout_panel_unique_ptr = std::unique_ptr<core::dock_layout_panel>;
    using label_unique_ptr = std::unique_ptr<core::label>;
    using memory_editor_unique_ptr = std::unique_ptr<core::memory_editor>;
    using notebook_unique_ptr = std::unique_ptr<core::notebook>;
    using panel_unique_ptr = std::unique_ptr<core::panel>;
    using pick_list_unique_ptr = std::unique_ptr<core::pick_list>;
    using text_editor_unique_ptr = std::unique_ptr<core::text_editor>;
    using textbox_unique_ptr = std::unique_ptr<core::textbox>;
    using column_pick_list_unique_ptr = std::unique_ptr<core::column_pick_list>;
    using loadable_view_unique_ptr = std::unique_ptr<core::loadable_view>;

    class view_factory {
    public:
        static caret_unique_ptr create_caret(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            const core::rect& bounds = {});

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
            const padding& padding = {},
            const core::rect& bounds = {});

        static document_footer_unique_ptr create_document_footer(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            dock::styles dock_style = dock::styles::fill,
            const padding& margin = {},
            const padding& padding = {});

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

        static memory_editor_unique_ptr create_memory_editor(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            dock::styles dock_style = dock::styles::fill,
            const padding& margin = {},
            const padding& padding = {},
            const core::rect& bounds = {});

        static loadable_view_unique_ptr create_loadable_view(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            core::result& result,
            const boost::filesystem::path& path,
            const padding& margin = {},
            const padding& padding = {});


        static column_pick_list_unique_ptr create_column_pick_list(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            dock::styles dock_style = dock::styles::fill,
            const padding& margin = {},
            const padding& padding = {},
            const core::rect& bounds = {});

        static panel_unique_ptr create_panel(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            dock::styles dock_style = dock::styles::fill,
            const padding& margin = {},
            const padding& padding = {},
            const core::rect& bounds = {});

        static dock_layout_panel_unique_ptr create_dock_layout_panel(
            core::state* state,
            const std::string& name,
            uint8_t fg_color,
            uint8_t bg_color,
            dock::styles dock_style = dock::styles::fill,
            const padding& margin = {},
            const padding& padding = {});

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

