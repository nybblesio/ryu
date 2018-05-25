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
#include <core/views/text_box.h>
#include <core/views/list_box.h>
#include <core/views/note_book.h>
#include <core/views/check_box.h>
#include <core/views/pick_list.h>
#include <core/views/text_editor.h>
#include <core/views/state_header.h>
#include <core/views/memory_editor.h>
#include <core/views/loadable_view.h>
#include <core/views/palette_editor.h>
#include <core/views/document_footer.h>
#include <core/views/column_pick_list.h>
#include <core/views/palette_entry_editor.h>
#include "core_types.h"

namespace ryu::core {

    using button_unique_ptr = std::unique_ptr<core::button>;
    using caret_unique_ptr = std::unique_ptr<core::caret>;
    using state_header_unique_ptr = std::unique_ptr<core::state_header>;
    using document_footer_unique_ptr = std::unique_ptr<core::document_footer>;
    using memory_editor_unique_ptr = std::unique_ptr<core::memory_editor>;
    using console_unique_ptr = std::unique_ptr<core::console>;
    using check_box_unique_ptr = std::unique_ptr<core::check_box>;
    using label_unique_ptr = std::unique_ptr<core::label>;
    using memory_editor_unique_ptr = std::unique_ptr<core::memory_editor>;
    using note_book_unique_ptr = std::unique_ptr<core::note_book>;
    using panel_unique_ptr = std::unique_ptr<core::panel>;
    using pick_list_unique_ptr = std::unique_ptr<core::pick_list>;
    using text_editor_unique_ptr = std::unique_ptr<core::text_editor>;
    using text_box_unique_ptr = std::unique_ptr<core::text_box>;
    using column_pick_list_unique_ptr = std::unique_ptr<core::column_pick_list>;
    using loadable_view_unique_ptr = std::unique_ptr<core::loadable_view>;
    using palette_entry_editor_unique_ptr = std::unique_ptr<core::palette_entry_editor>;
    using palette_editor_unique_ptr = std::unique_ptr<core::palette_editor>;
    using list_box_unique_ptr = std::unique_ptr<core::list_box>;

    class view_factory {
    public:
        template <
                typename T,
                typename = std::enable_if<std::is_base_of<core::view, T>::value>>
        static std::unique_ptr<T> create_view(
                core::view_host* host,
                const std::string& name,
                core::font_family* family,
                core::palette* pal,
                uint8_t fg_color,
                uint8_t bg_color,
                const std::string& value = "",
                core::dock::styles dock_style = dock::styles::fill,
                const core::padding& margin = {},
                const core::padding& padding = {},
                const core::rect& bounds = {}) {
            auto view = std::make_unique<T>(name, host);
            view->dock(dock_style);
            view->fg_color(fg_color);
            view->bg_color(bg_color);
            view->palette(pal);
            view->margin(margin);
            view->padding(padding);
            view->bounds(bounds);
            view->font_family(family);
            view->value(value);
            view->initialize();
            return view;
        }

        static pick_list_unique_ptr create_pick_list(
            core::view_host* host,
            const std::string& name,
            core::font_family* family,
            core::palette* pal,
            uint8_t fg_color,
            uint8_t bg_color,
            const option_list& options = {},
            dock::styles dock_style = dock::styles::left,
            const padding& margin = {},
            const padding& padding = {},
            const core::rect& bounds = {});

        static console_unique_ptr create_console(
            core::view_host* host,
            const std::string& name,
            core::font_family* family,
            core::palette* pal,
            uint8_t fg_color,
            uint8_t bg_color,
            const core::code_to_attr_dict& mapper,
            dock::styles dock_style = dock::styles::fill,
            const padding& margin = {},
            const padding& padding = {},
            const core::rect& bounds = {});

        static text_editor_unique_ptr create_text_editor(
            core::view_host* host,
            const std::string& name,
            core::font_family* family,
            core::palette* pal,
            uint8_t fg_color,
            uint8_t bg_color,
            uint32_t rows,
            uint16_t columns,
            dock::styles dock_style = dock::styles::fill,
            const padding& margin = {},
            const padding& padding = {},
            const core::rect& bounds = {});

        static loadable_view_unique_ptr create_loadable_view(
            core::view_host* host,
            const std::string& name,
            core::font_family* family,
            core::palette* pal,
            core::preferences* prefs,
            uint8_t fg_color,
            uint8_t bg_color,
            core::result& result,
            const boost::filesystem::path& path,
            dock::styles dock_style = dock::styles::fill,
            const padding& margin = {},
            const padding& padding = {});
    };

};