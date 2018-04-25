//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "state.h"
#include "view_factory.h"

namespace ryu::core {

    pick_list_unique_ptr view_factory::create_pick_list(
            core::view_host* host,
            const std::string& name,
            core::font_family* family,
            core::palette* pal,
            uint8_t fg_color,
            uint8_t bg_color,
            const option_list& options,
            core::dock::styles dock_style,
            const core::padding& margin,
            const core::padding& padding,
            const core::rect& bounds) {
        auto view = create_view<pick_list>(
                host,
                name,
                family,
                pal,
                fg_color,
                bg_color,
                "",
                dock_style,
                margin,
                padding,
                bounds);

        auto& list_options = view->options();
        for (const auto& option : options)
            list_options.push_back(option);

        return view;
    }

    console_unique_ptr view_factory::create_console(
            core::view_host* host,
            const std::string& name,
            core::font_family* family,
            core::palette* pal,
            uint8_t fg_color,
            uint8_t bg_color,
            const core::code_to_attr_dict& mapper,
            core::dock::styles dock_style,
            const core::padding& margin,
            const core::padding& padding,
            const core::rect& bounds) {
        auto view = create_view<console>(
                host,
                name,
                family,
                pal,
                fg_color,
                bg_color,
                "",
                dock_style,
                margin,
                padding,
                bounds);
        view->code_mapper(mapper);
        return view;
    }

    text_editor_unique_ptr view_factory::create_text_editor(
            core::view_host* host,
            const std::string& name,
            core::font_family* family,
            core::palette* pal,
            uint8_t fg_color,
            uint8_t bg_color,
            uint32_t rows,
            uint16_t columns,
            dock::styles dock_style,
            const padding& margin,
            const padding& padding,
            const rect& bounds) {
        auto view = create_view<text_editor>(
                host,
                name,
                family,
                pal,
                fg_color,
                bg_color,
                "",
                dock_style,
                margin,
                padding,
                bounds);
        view->size(rows, columns);
        return view;
    }

    loadable_view_unique_ptr view_factory::create_loadable_view(
            core::view_host* host,
            const std::string& name,
            core::font_family* family,
            core::palette* pal,
            core::preferences* prefs,
            uint8_t fg_color,
            uint8_t bg_color,
            core::result& result,
            const boost::filesystem::path& path,
            dock::styles dock_style,
            const padding& margin,
            const padding& padding) {
        auto view = create_view<loadable_view>(
                host,
                name,
                family,
                pal,
                fg_color,
                bg_color,
                "",
                dock_style,
                margin,
                padding,
                {});

        fs::path view_path = path;
        if (!view_path.is_absolute())
            view_path = prefs->executable_path().append(view_path.string());

        if (view->load(result, view_path))
            return view;
        return nullptr;
    }

}