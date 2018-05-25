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

#include <cstdint>
#include <yaml-cpp/yaml.h>
#include <boost/filesystem.hpp>
#include "result.h"

namespace ryu::core {

    namespace fs = boost::filesystem;

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

    node_result_t get_node_from_path(
        const YAML::Node& root,
        const std::string& path);

    bool expand_includes(
        core::result& result,
        std::stringstream& stream,
        const fs::path& path);

    constant_result_t get_constant_int(
        const YAML::Node& constants_node,
        const std::string& path);

    template <typename T>
    bool get_optional(YAML::Node node, T& var) {
        if (node == nullptr)
            return false;
        var = node.as<T>();
        return true;
    }

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

}