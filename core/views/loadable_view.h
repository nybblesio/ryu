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

    class loadable_view : public core::dock_layout_panel {
    public:
        enum class types {
            label = 0,
            text_box,
            text_editor,
            pick_list,
            column_pick_list,
            panel,
            dock_panel,
            notebook,
            state_header,
            none
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
        bool create_views(
                core::result& result,
                YAML::Node root,
                YAML::Node views_node,
                core::view* parent_view);

        bool expand_includes(
                core::result& result,
                std::stringstream& stream,
                const boost::filesystem::path& path);

        constant_result_t get_constant_int(
                YAML::Node constants_node,
                const std::string& path);

    private:
        std::string _meta_name;
        std::vector<std::unique_ptr<view>> _views {};
    };

};

