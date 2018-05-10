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
#include <core/yaml_support.h>
#include <boost/filesystem.hpp>
#include "panel.h"

namespace ryu::core {

    struct view_tab_stop_t {
        core::view* view = nullptr;
        std::string prev {};
        std::string next {};
    };

    class loadable_view : public core::panel {
    public:
        enum class types {
            label = 0,
            button,
            text_box,
            text_editor,
            pick_list,
            column_pick_list,
            panel,
            integrated_circuit_editor,
            notebook,
            state_header,
            check_box,
            caret,
            console,
            document_footer,
            loadable_view,
            memory_editor,
            palette_entry_editor,
            palette_editor,
            list_box,
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

    private:
        void get_layout_config(
            core::result& result,
            const YAML::Node& root,
            const YAML::Node& node,
            core::view* v);

        bool create_views(
            core::result& result,
            const YAML::Node& root,
            const YAML::Node& views_node,
            core::view* parent_view);

    private:
        std::string _meta_name;
        std::vector<view_tab_stop_t> _tab_stops {};
        std::vector<std::unique_ptr<view>> _views {};
    };

};