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

#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include <yaml-cpp/yaml.h>
#include <boost/filesystem.hpp>
#include "dock_layout_panel.h"

namespace ryu::core {

    struct constant_result_t {
        bool found = false;
        int32_t value = 0;
        std::string error_message {};
    };

    struct node_result_t {
        bool found = false;
        YAML::Node node {};
        std::string error_message {};
    };

    struct view_tab_stop_t {
        core::view* view = nullptr;
        std::string prev {};
        std::string next {};
    };

    class loadable_view : public core::dock_layout_panel {
    public:
        enum class types {
            label = 0,
            button,
            text_box,
            text_editor,
            pick_list,
            column_pick_list,
            panel,
            dock_panel,
            notebook,
            state_header,
            check_box,
            caret,
            console,
            document_footer,
            loadable_view,
            memory_editor,
            none
        };

        enum class sources {
            none,
            displays,
            components,
            circuits,
            machines
        };

        enum class input_filters {
            none,
            hex,
            decimal,
            identifier,
            string
        };

        loadable_view(
            const std::string& name,
            core::view_host* host);

        ~loadable_view() override;

        bool load(
            core::result& result,
            const boost::filesystem::path& path);

    protected:
        void on_initialize() override;

        template <typename T>
        bool get_constant(
                core::result& result,
                YAML::Node root,
                YAML::Node node,
                T& var) {
            if (!node.IsNull() && node.IsScalar()) {
                auto constant_result = get_constant_int(root["constants"], node.as<std::string>());
                if (constant_result.found) {
                    var = static_cast<T>(constant_result.value);
                } else {
                    result.add_message(
                            "P009",
                            constant_result.error_message,
                            true);
                    return false;
                }
            }
            return true;
        }

        template <typename T>
        bool get_optional(YAML::Node node, T& var) {
            if (node == nullptr)
                return false;
            var = node.as<T>();
            return true;
        }

    private:
        bool create_views(
                core::result& result,
                const YAML::Node& root,
                const YAML::Node& views_node,
                core::view* parent_view);

        bool expand_includes(
                core::result& result,
                std::stringstream& stream,
                const boost::filesystem::path& path);

        node_result_t get_node_from_path(
                const YAML::Node& root,
                const std::string& path);

        constant_result_t get_constant_int(
                const YAML::Node& constants_node,
                const std::string& path);

    private:
        std::string _meta_name;
        std::vector<view_tab_stop_t> _tab_stops {};
        std::vector<std::unique_ptr<view>> _views {};
    };

};

