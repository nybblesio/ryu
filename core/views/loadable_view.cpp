//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <core/state.h>
#include <yaml-cpp/yaml.h>
#include <logger_factory.h>
#include <core/view_factory.h>
#include <hardware/registry.h>
#include "loadable_view.h"

namespace ryu::core {

    static logger* s_log = logger_factory::instance()->create(
            "loadable_view",
            logger::level::info);

    loadable_view::loadable_view(
            const std::string& name,
            core::view_host* host) : dock_layout_panel(name, host) {
    }

    loadable_view::~loadable_view() {
    }

    bool loadable_view::load(
            core::result& result,
            const boost::filesystem::path& path) {
        clear_children();
        _views.clear();
        _tab_stops.clear();
        _meta_name.clear();

        std::stringstream expanded_stream;
        if (!expand_includes(result, expanded_stream, path))
            return false;

        s_log->info(fmt::format("expanded_stream = {}", expanded_stream.str()));

        auto root = YAML::Load(expanded_stream);
        if (!root.IsNull() && root.IsMap()) {
            auto name_node = root["name"];
            if (!name_node.IsNull() && name_node.IsScalar()) {
                _meta_name = name_node.as<std::string>();
            }

            auto views_node = root["views"];
            if (!create_views(result, root, views_node, this))
                return false;

            for (const auto& tab_stop : _tab_stops) {
                auto next_view = find_by_name<core::view>(tab_stop.next);
                if (next_view != nullptr)
                    tab_stop.view->next_view(next_view);

                auto prev_view = find_by_name<core::view>(tab_stop.prev);
                if (prev_view != nullptr)
                    tab_stop.view->prev_view(prev_view);
            }

            std::string focus_view_name;
            if (get_optional_scalar(root["focus"], focus_view_name)) {
                auto view = find_by_name<core::view>(focus_view_name);
                if (view != nullptr)
                    focus(view);
            }
        } else {
            result.add_message(
                "P003",
                "expected root node of type map",
                true);
        }

        return !result.is_failed();
    }

    bool loadable_view::create_views(
            core::result& result,
            YAML::Node root,
            YAML::Node views_node,
            core::view* parent_view) {
        if (views_node.IsNull() || !views_node.IsSequence())
            return true;

        for (auto view_node_it = views_node.begin();
                 view_node_it != views_node.end();
                 ++view_node_it) {
            YAML::Node view_node = *view_node_it;
            if (!view_node.IsMap())
                continue;

            std::string name;
            std::string value;
            bool visible = true;
            bool enabled = false;
            bool tab_stop = false;
            core::rect bounds = {};
            core::padding pads = {};
            types type = types::none;
            core::padding margins = {};
            std::string prev_view_name;
            std::string next_view_name;
            uint8_t fg_color = parent_view->fg_color();
            uint8_t bg_color = parent_view->bg_color();
            dock::styles dock_style = dock::styles::none;
            border::types border_type = border::types::none;
            view::sizing::types sizing = view::sizing::types::content;

            std::unique_ptr<view> current_view;

            if (!get_constant(result, root, view_node["type"], type))
                return false;

            get_optional_scalar(view_node["name"], name);
            get_optional_scalar(view_node["value"], value);
            get_optional_scalar(view_node["enabled"], enabled);
            get_optional_scalar(view_node["visible"], visible);
            get_optional_scalar(view_node["tab-stop"], tab_stop);
            get_optional_scalar(view_node["next"], next_view_name);
            get_optional_scalar(view_node["prev"], prev_view_name);

            get_constant(result, root, view_node["dock"], dock_style);
            get_constant(result, root, view_node["sizing"], sizing);
            get_constant(result, root, view_node["border"], border_type);
            get_padding(result, view_node["margin"], margins);
            get_padding(result, view_node["padding"], pads);
            get_bounds(result, view_node["bounds"], bounds);

            auto fg_result = get_node_from_path(view_node, "colors.fg");
            if (fg_result.found)
                get_constant(result, root, fg_result.node, fg_color);

            auto bg_result = get_node_from_path(view_node, "colors.bg");
            if (bg_result.found)
                get_constant(result, root, bg_result.node, bg_color);

            switch (type) {
                case types::label: {
                    current_view = core::view_factory::create_view<core::label>(
                            host(),
                            name,
                            font_family(),
                            palette(),
                            fg_color,
                            bg_color,
                            value,
                            dock_style,
                            margins,
                            pads,
                            bounds);
                    break;
                }
                case types::button: {
                    auto button = core::view_factory::create_view<core::button>(
                            host(),
                            name,
                            font_family(),
                            palette(),
                            fg_color,
                            bg_color,
                            value,
                            dock_style,
                            margins,
                            pads,
                            bounds);

                    uint32_t sc_color;
                    auto sc_result = get_node_from_path(view_node, "shortcut-color");
                    if (sc_result.found)
                        if (get_constant(result, root, sc_result.node, sc_color))
                            button->shortcut_color(static_cast<uint8_t>(sc_color));

                    std::string shortcut;
                    if (get_optional_scalar(view_node["shortcut"], shortcut)) {
                        button->shortcut(shortcut);
                    }

                    uint32_t width;
                    if (get_optional_scalar(view_node["width"], width)) {
                        button->width(width);
                    }

                    current_view = std::move(button);
                    break;
                }
                case types::text_box: {
                    auto text_box = core::view_factory::create_view<core::text_box>(
                            host(),
                            name,
                            font_family(),
                            palette(),
                            fg_color,
                            bg_color,
                            value,
                            dock_style,
                            margins,
                            pads,
                            bounds);

                    uint32_t size;
                    auto size_result = get_node_from_path(view_node, "size.width");
                    if (size_result.found) {
                        if (get_optional_scalar(size_result.node, size))
                            text_box->width(static_cast<uint8_t>(size));
                    }

                    size_result = get_node_from_path(view_node, "size.length");
                    if (size_result.found) {
                        if (get_optional_scalar(size_result.node, size))
                            text_box->length(static_cast<uint16_t>(size));
                    }

                    input_filters filter = input_filters::none;
                    if (get_constant(result, root, view_node["input-filter"], filter)) {
                        switch (filter) {
                            case input_filters::hex:
                                text_box->on_key_down([](int key_code) {
                                    return isxdigit(key_code);
                                });
                                break;
                            default:
                                break;
                        }
                    }

                    current_view = std::move(text_box);
                    break;
                }
                case types::text_editor: {
                    uint32_t document_columns = 40, document_rows = 25;
                    auto document_rows_result = get_node_from_path(
                            view_node,
                            "document.rows");
                    if (document_rows_result.found)
                        get_optional_scalar(
                                document_rows_result.node,
                                document_rows);
                    auto document_columns_result = get_node_from_path(
                            view_node,
                            "document.columns");
                    if (document_columns_result.found)
                        get_optional_scalar(
                                document_columns_result.node,
                                document_columns);

                    auto text_editor = core::view_factory::create_text_editor(
                            host(),
                            name,
                            font_family(),
                            palette(),
                            fg_color,
                            bg_color,
                            document_rows,
                            static_cast<uint16_t>(document_columns),
                            dock_style,
                            margins,
                            pads,
                            bounds);

                    uint32_t page_columns = 40, page_rows = 25;
                    auto page_rows_result = get_node_from_path(
                            view_node,
                            "page-size.rows");
                    if (page_rows_result.found)
                        get_optional_scalar(
                                page_rows_result.node,
                                page_rows);
                    auto page_columns_result = get_node_from_path(
                            view_node,
                            "page-size.columns");
                    if (page_columns_result.found)
                        get_optional_scalar(
                                page_columns_result.node,
                                page_columns);

                    text_editor->page_size(
                            static_cast<uint8_t>(page_rows),
                            static_cast<uint8_t>(page_columns));

                    uint8_t editor_color;
                    if (get_constant(result, root, view_node["caret-color"], editor_color))
                        text_editor->caret_color(editor_color);

                    if (get_constant(result, root, view_node["selection-color"], editor_color))
                        text_editor->selection_color(editor_color);

                    if (get_constant(result, root, view_node["line-number-color"], editor_color))
                        text_editor->line_number_color(editor_color);

                    current_view = std::move(text_editor);
                    break;
                }
                case types::pick_list: {
                    option_list options {};
                    sources source = sources::none;
                    if (!get_constant(result, root, view_node["source"], source)) {
                        auto options_node = view_node["options"];
                        if (!options_node.IsNull() && options_node.IsSequence()) {
                            options = options_node.as<option_list>();
                        }
                    } else {
                        switch (source) {
                            case sources::displays: {
                                for (auto& display : hardware::display::catalog())
                                    options.push_back(display.name());
                                break;
                            }
                            case sources::components: {
                                break;
                            }
                            case sources::circuits: {
                                auto base_type = rttr::type::get<ryu::hardware::integrated_circuit>();
                                auto ic_types = base_type.get_derived_classes();
                                for (const auto& rttr_type : ic_types) {
                                    auto ic_type_name = rttr_type
                                        .get_metadata(ryu::hardware::meta_data_key::type_name)
                                        .to_string();
                                    options.push_back(ic_type_name);
                                }
                                break;
                            }
                            case sources::machines: {
                                for (const auto machine : hardware::registry::instance()->machines()) {
                                    options.push_back(machine->name());
                                }
                                break;
                            }
                            default:
                                break;
                        }
                    }
                    auto pick_list = core::view_factory::create_pick_list(
                            host(),
                            name,
                            font_family(),
                            palette(),
                            fg_color,
                            bg_color,
                            options,
                            dock_style,
                            margins,
                            pads,
                            bounds);
                    current_view = std::move(pick_list);
                    break;
                }
                case types::column_pick_list: {
                    auto column_pick_list = core::view_factory::create_view<core::column_pick_list>(
                            host(),
                            name,
                            font_family(),
                            palette(),
                            fg_color,
                            bg_color,
                            value,
                            dock_style,
                            margins,
                            pads,
                            bounds);
                    column_pick_list->border(border_type);
                    auto headers_node = view_node["headers"];
                    if (!headers_node.IsNull() && headers_node.IsSequence()) {
                        for (auto header_node_it = headers_node.begin();
                                 header_node_it != headers_node.end();
                                 ++header_node_it) {
                            YAML::Node header_node = *header_node_it;

                            std::string title;
                            uint32_t width = 100;
                            uint8_t col_fg_color = fg_color;
                            uint8_t col_bg_color = bg_color;
                            column_pick_list::valign_t valign = column_pick_list::valign_t::none;
                            column_pick_list::halign_t halign = column_pick_list::halign_t::none;

                            get_optional_scalar(header_node["title"], title);
                            get_optional_scalar(header_node["width"], width);

                            auto col_fg_result = get_node_from_path(header_node, "colors.fg");
                            if (col_fg_result.found)
                                get_constant(result, root, col_fg_result.node, col_fg_color);

                            auto col_bg_result = get_node_from_path(header_node, "colors.bg");
                            if (col_bg_result.found)
                                get_constant(result, root, col_bg_result.node, col_bg_color);

                            auto halign_result = get_node_from_path(header_node, "alignment.horizontal");
                            if (halign_result.found)
                                get_constant(result, root, halign_result.node, halign);

                            auto valign_result = get_node_from_path(header_node, "alignment.vertical");
                            if (valign_result.found)
                                get_constant(result, root, valign_result.node, valign);

                            column_pick_list->add_header(
                                    title,
                                    col_fg_color,
                                    col_bg_color,
                                    width,
                                    halign,
                                    valign);
                        }
                    }
                    current_view = std::move(column_pick_list);
                    break;
                }
                case types::panel: {
                    auto panel = core::view_factory::create_view<core::panel>(
                            host(),
                            name,
                            font_family(),
                            palette(),
                            fg_color,
                            bg_color,
                            value,
                            dock_style,
                            margins,
                            pads,
                            bounds);
                    panel->border(border_type);
                    current_view = std::move(panel);
                    break;
                }
                case types::dock_panel: {
                    auto panel = core::view_factory::create_view<core::dock_layout_panel>(
                            host(),
                            name,
                            font_family(),
                            palette(),
                            fg_color,
                            bg_color,
                            value,
                            dock_style,
                            margins,
                            pads,
                            bounds);
                    panel->border(border_type);
                    current_view = std::move(panel);
                    break;
                }
                case types::notebook: {
                    auto notebook = core::view_factory::create_view<core::note_book>(
                            host(),
                            name,
                            font_family(),
                            palette(),
                            fg_color,
                            bg_color,
                            value,
                            dock_style,
                            margins,
                            pads,
                            bounds);
                    // XXX: there are some issues with the way note_book works:
                    //          each tab is a child view of the note_book, this presents an ordering problem
                    //          should we just treat each sub-view of the note_book as a tab and the
                    //          value is the title of the tab?
                    current_view = std::move(notebook);
                    break;
                }
                case types::state_header: {
                    std::string state_name;
                    uint8_t state_color = fg_color;
                    get_optional_scalar(view_node["state"], state_name);
                    if (!get_constant(result, root, view_node["state_color"], state_color))
                        return false;
                    auto state_header = core::view_factory::create_view<core::state_header>(
                            host(),
                            name,
                            font_family(),
                            palette(),
                            fg_color,
                            bg_color,
                            value,
                            dock_style,
                            margins,
                            pads,
                            bounds);
                    state_header->state(state_name);
                    state_header->state_color(state_color);
                    current_view = std::move(state_header);
                    break;
                }
                case types::check_box: {
                    current_view = core::view_factory::create_view<core::check_box>(
                            host(),
                            name,
                            font_family(),
                            palette(),
                            fg_color,
                            bg_color,
                            value,
                            dock_style,
                            margins,
                            pads,
                            bounds);
                    break;
                }
                case types::caret: {
                    auto caret = core::view_factory::create_view<core::caret>(
                            host(),
                            name,
                            font_family(),
                            palette(),
                            fg_color,
                            bg_color,
                            value,
                            dock_style,
                            margins,
                            pads,
                            bounds);
                    // XXX: need to support page size here?
                    current_view = std::move(caret);
                    break;
                }
                case types::console: {
                    auto console = core::view_factory::create_view<core::console>(
                            host(),
                            name,
                            font_family(),
                            palette(),
                            fg_color,
                            bg_color,
                            value,
                            dock_style,
                            margins,
                            pads,
                            bounds);

                    uint8_t editor_color;
                    if (get_constant(result, root, view_node["caret-color"], editor_color))
                        console->caret_color(editor_color);

                    if (get_constant(result, root, view_node["selection-color"], editor_color))
                        console->selection_color(editor_color);

                    current_view = std::move(console);
                    break;
                }
                case types::document_footer: {
                    auto document_footer = core::view_factory::create_view<core::document_footer>(
                            host(),
                            name,
                            font_family(),
                            palette(),
                            fg_color,
                            bg_color,
                            value,
                            dock_style,
                            margins,
                            pads,
                            bounds);
                    current_view = std::move(document_footer);
                    break;
                }
                case types::loadable_view: {
                    std::string view_path;
                    if (!get_optional_scalar(view_node["path"], view_path))
                        return false;
                    auto loadable = core::view_factory::create_loadable_view(
                            host(),
                            name,
                            font_family(),
                            palette(),
                            host()->prefs(),
                            fg_color,
                            bg_color,
                            result,
                            view_path,
                            dock_style,
                            margins,
                            pads);
                    current_view = std::move(loadable);
                    break;
                }
                case types::memory_editor: {
                    auto memory_editor = core::view_factory::create_view<core::memory_editor>(
                            host(),
                            name,
                            font_family(),
                            palette(),
                            fg_color,
                            bg_color,
                            value,
                            dock_style,
                            margins,
                            pads,
                            bounds);

                    uint8_t editor_color;
                    if (get_constant(result, root, view_node["caret-color"], editor_color))
                        memory_editor->caret_color(editor_color);

                    if (get_constant(result, root, view_node["selection-color"], editor_color))
                        memory_editor->selection_color(editor_color);

                    if (get_constant(result, root, view_node["address-color"], editor_color))
                        memory_editor->address_color(editor_color);

                    current_view = std::move(memory_editor);
                    break;
                }
                default: {
                    result.add_message(
                            "P099",
                            fmt::format("unknown view type for '{}'", name),
                            true);
                    return false;
                }
            }

            current_view->tab_stop(tab_stop);
            if (tab_stop) {
                _tab_stops.push_back(view_tab_stop_t {
                        current_view.get(),
                        prev_view_name,
                        next_view_name});
            }

            current_view->sizing(sizing);
            current_view->enabled(enabled);
            current_view->visible(visible);
            parent_view->add_child(current_view.get());

            auto sub_views_node = view_node["views"];
            if (!create_views(result, root, sub_views_node, current_view.get()))
                return false;

            _views.push_back(std::move(current_view));
        }

        return true;
    }

    void loadable_view::on_initialize() {
    }

    bool loadable_view::expand_includes(
            core::result& result,
            std::stringstream& stream,
            const boost::filesystem::path& path) {
        std::stringstream temp_stream;
        try {
            std::ifstream file(path.string(), std::ios::in);
            if (file.is_open()) {
                temp_stream << file.rdbuf();
                file.close();
            }
        } catch (std::exception& e) {
            result.add_message(
                "P001",
                fmt::format("unable to read view file: {}", e.what()),
                true);
            return false;
        }
        std::string line;
        while (std::getline(temp_stream, line)) {
            if (line[0] == '!') {
                auto parts = string_to_list(line, ' ');
                if (parts.size() < 2) {
                    result.add_message(
                        "P002",
                        "!include directive requires file path",
                        true);
                    return false;
                }
                // XXX: this is a hack, come back and fix it
                boost::filesystem::path include_path(parts[1]);
                if (!include_path.has_root_path()) {
                    boost::filesystem::path temp_path(path);
                    temp_path.remove_filename();
                    include_path = temp_path.append(include_path.filename().string());
                }
                if (!expand_includes(result, stream, include_path.string()))
                    return false;
            } else {
                stream << line << "\n";
            }
        }
        return true;
    }

    bool loadable_view::get_padding(
            core::result& result,
            YAML::Node node,
            core::padding& pads) {
        if (!node.IsNull() && node.IsSequence()) {
            auto values = node.as<std::vector<int32_t>>();
            if (values.size() < 4) {
                result.add_message(
                        "P010",
                        "pad/margin require four values: [<left>, <right>, <top>, <bottom>].",
                        true);
                return false;
            }
            pads.left(values[0]);
            pads.right(values[1]);
            pads.top(values[2]);
            pads.bottom(values[3]);
        }
        return true;
    }

    bool loadable_view::get_bounds(
            core::result& result,
            YAML::Node node,
            core::rect& bounds) {
        if (!node.IsNull() && node.IsSequence()) {
            auto values = node.as<std::vector<int32_t>>();
            if (values.size() < 4) {
                result.add_message(
                        "P010",
                        "bounds require four values: [<left>, <top>, <width>, <height>].",
                        true);
                return false;
            }
            bounds.left(values[0]);
            bounds.top(values[1]);
            bounds.width(values[2]);
            bounds.height(values[3]);
        }
        return true;
    }

    constant_result_t loadable_view::get_constant_int(
            YAML::Node constants_node,
            const std::string& path) {
        constant_result_t result {};
        auto node_result = get_node_from_path(constants_node, path);
        if (node_result.found) {
            result.found = true;
            result.value = node_result.node.as<int32_t>();

        } else {
            result.error_message = node_result.error_message;
        }
        return result;
    }

    node_result_t loadable_view::get_node_from_path(
            YAML::Node root,
            const std::string& path) {
        auto parts = string_to_list(path, '.');
        std::vector<YAML::Node> nodes {root};
        for (size_t i = 0; i < parts.size(); i++) {
            auto next_node = nodes.back()[parts[i]];
            if (next_node == nullptr) {
                node_result_t result {};
                result.error_message = fmt::format(
                        "unable to find node '{}' for path '{}'.",
                        parts[i],
                        path);
                return result;
            }
            nodes.push_back(next_node);
        }
        return node_result_t {true, nodes.back()};
    }

}