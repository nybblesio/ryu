//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <logger_factory.h>
#include <yaml-cpp/yaml.h>
#include <ide/ide_types.h>
#include <core/state.h>
#include <core/view_factory.h>
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
            auto view_node = *view_node_it;
            if (!view_node.IsMap())
                continue;

            std::string name;
            bool enabled = false;
            types type = types::none;
            core::padding pads = {};
            core::padding margins = {};
            dock::styles dock_style = dock::styles::none;
            ide::colors::indexes fg_color = ide::colors::info_text;
            ide::colors::indexes bg_color = ide::colors::fill_color;
            view::sizing::types sizing = view::sizing::types::content;

            std::unique_ptr<view> current_view;

            auto view_name_node = view_node["name"];
            if (!view_name_node.IsNull() && view_name_node.IsScalar()) {
                name = view_name_node.as<std::string>();
            }

            auto view_type_node = view_node["type"];
            if (!view_type_node.IsNull() && view_type_node.IsScalar()) {
                auto constant_result = get_constant_int(
                        root["constants"],
                        view_type_node.as<std::string>());
                if (constant_result.found) {
                    type = static_cast<types>(constant_result.value);
                } else {
                    result.add_message(
                            "P009",
                            constant_result.error_message,
                            true);
                    return false;
                }
            }

            auto view_dock_node = view_node["dock"];
            if (!view_dock_node.IsNull() && view_dock_node.IsScalar()) {
                auto constant_result = get_constant_int(
                        root["constants"],
                        view_dock_node.as<std::string>());
                if (constant_result.found) {
                    dock_style = static_cast<dock::styles>(constant_result.value);
                } else {
                    result.add_message(
                            "P009",
                            constant_result.error_message,
                            true);
                    return false;
                }
            }

            auto view_margin_node = view_node["margin"];
            if (!view_margin_node.IsNull() && view_margin_node.IsSequence()) {
                auto values = view_margin_node.as<std::vector<int32_t>>();
                if (values.size() < 4) {
                    result.add_message(
                            "P010",
                            "margins require four values: [<left>, <right>, <top>, <bottom>].",
                            true);
                    return false;
                }
                margins.left(values[0]);
                margins.right(values[1]);
                margins.top(values[2]);
                margins.bottom(values[3]);
            }

            auto view_padding_node = view_node["padding"];
            if (!view_padding_node.IsNull() && view_padding_node.IsSequence()) {
                auto values = view_padding_node.as<std::vector<int32_t>>();
                if (values.size() < 4) {
                    result.add_message(
                            "P010",
                            "paddings require four values: [<left>, <right>, <top>, <bottom>].",
                            true);
                    return false;
                }
                pads.left(values[0]);
                pads.right(values[1]);
                pads.top(values[2]);
                pads.bottom(values[3]);
            }

            auto view_colors_node = view_node["colors"];
            if (!view_colors_node.IsNull() && view_colors_node.IsMap()) {
                auto fg_color_node = view_colors_node["fg"];
                if (!fg_color_node.IsNull() && fg_color_node.IsScalar()) {
                    auto constant_result = get_constant_int(
                            root["constants"],
                            fg_color_node.as<std::string>());
                    if (constant_result.found) {
                        fg_color = static_cast<ide::colors::indexes>(constant_result.value);
                    } else {
                        result.add_message(
                                "P009",
                                constant_result.error_message,
                                true);
                        return false;
                    }
                }
                auto bg_color_node = view_colors_node["bg"];
                if (!bg_color_node.IsNull() && bg_color_node.IsScalar()) {
                    auto constant_result = get_constant_int(
                            root["constants"],
                            bg_color_node.as<std::string>());
                    if (constant_result.found) {
                        bg_color = static_cast<ide::colors::indexes>(constant_result.value);
                    } else {
                        result.add_message(
                                "P009",
                                constant_result.error_message,
                                true);
                        return false;
                    }
                }
            }

            auto enabled_node = view_node["enabled"];
            if (!enabled_node.IsNull() && enabled_node.IsScalar()) {
                enabled = enabled_node.as<bool>();
            }

            auto view_sizing_node = view_node["sizing"];
            if (!view_sizing_node.IsNull() && view_sizing_node.IsScalar()) {
                auto constant_result = get_constant_int(
                        root["constants"],
                        view_sizing_node.as<std::string>());
                if (constant_result.found) {
                    sizing = static_cast<view::sizing::types>(constant_result.value);
                } else {
                    result.add_message(
                            "P009",
                            constant_result.error_message,
                            true);
                    return false;
                }
            }

            // XXX: fix the signatures on the view_factory after this is done
            //          so we don't have to cast back and forth
            switch (type) {
                case types::label: {
                    std::string value;
                    auto value_node = view_node["value"];
                    if (!value_node.IsNull() && value_node.IsScalar()) {
                        value = value_node.as<std::string>();
                    }
                    current_view = core::view_factory::create_label(
                            dynamic_cast<core::state*>(host()),
                            name,
                            fg_color,
                            bg_color,
                            value,
                            dock_style,
                            margins,
                            pads);
                    break;
                }
                case types::text_box: {
                    break;
                }
                case types::text_editor: {
                    break;
                }
                case types::pick_list: {
                    break;
                }
                case types::column_pick_list: {
                    break;
                }
                case types::panel: {
                    break;
                }
                case types::dock_panel: {
                    current_view = core::view_factory::create_dock_layout_panel(
                            dynamic_cast<core::state*>(host()),
                            name,
                            fg_color,
                            bg_color,
                            dock_style,
                            margins,
                            pads);
                    break;
                }
                case types::notebook: {
                    break;
                }
                case types::state_header: {
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

            if (current_view != nullptr) {
                current_view->sizing(sizing);
                current_view->enabled(enabled);
                parent_view->add_child(current_view.get());

                auto sub_views_node = view_node["views"];
                if (!create_views(result, root, sub_views_node, current_view.get()))
                    return false;

                _views.push_back(std::move(current_view));
            }
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

    constant_result_t loadable_view::get_constant_int(
            YAML::Node constants_node,
            const std::string& path) {
        auto parts = string_to_list(path, '.');
        std::vector<YAML::Node> nodes {constants_node};
        for (size_t i = 0; i < parts.size(); i++) {
            auto next_node = nodes.back()[parts[i]];
            if (next_node == nullptr) {
                constant_result_t result {};
                result.error_message = fmt::format(
                        "unable to find node '{}' for path '{}'.",
                        parts[i],
                        path);
                return result;
            }
            nodes.push_back(next_node);
        }
        return constant_result_t {true, nodes.back().as<int32_t>()};
    }
}