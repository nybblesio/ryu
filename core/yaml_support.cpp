//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <fmt/format.h>
#include <common/string_support.h>
#include <common/stream_support.h>
#include "yaml_support.h"

namespace ryu::core {

    node_result_t get_node_from_path(
            const YAML::Node& root,
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

    bool expand_includes(
            core::result& result,
            std::stringstream& stream,
            const boost::filesystem::path& path) {
        std::stringstream temp_stream;
        if (!ryu::read_text(result, path, temp_stream))
            return false;

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

    constant_result_t get_constant_int(
            const YAML::Node& constants_node,
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

}