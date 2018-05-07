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
#include "core_types.h"
#include "padding.h"
#include "rect.h"

namespace YAML {
    template<>
    struct convert<ryu::core::context_window::sizes> {
        static Node encode(const ryu::core::context_window::sizes& rhs) {
            return Node(static_cast<uint32_t>(rhs));
        }

        static bool decode(const Node& node, ryu::core::context_window::sizes& rhs) {
            if (!node.IsScalar())
                return false;
            rhs = static_cast<ryu::core::context_window::sizes>(node.as<uint32_t>());
            return true;
        }
    };

    template<>
    struct convert<ryu::core::rect> {
        static Node encode(const ryu::core::rect& rhs) {
            Node node;
            node.push_back(rhs.left());
            node.push_back(rhs.top());
            node.push_back(rhs.width());
            node.push_back(rhs.height());
            return node;
        }

        static bool decode(const Node& node, ryu::core::rect& rhs) {
            if (!node.IsSequence() || node.size() < 4)
                return false;

            rhs.pos(node[0].as<uint32_t>(), node[1].as<uint32_t>());
            rhs.size(node[2].as<uint32_t>(), node[3].as<uint32_t>());
            return true;
        }
    };

    template<>
    struct convert<ryu::core::padding> {
        static Node encode(const ryu::core::padding& rhs) {
            Node node;
            node.push_back(rhs.left());
            node.push_back(rhs.right());
            node.push_back(rhs.top());
            node.push_back(rhs.bottom());
            return node;
        }

        static bool decode(const Node& node, ryu::core::padding& rhs) {
            if (!node.IsSequence() || node.size() < 4)
                return false;

            rhs.left(node[0].as<uint32_t>());
            rhs.right(node[1].as<uint32_t>());
            rhs.top(node[2].as<uint32_t>());
            rhs.bottom(node[3].as<uint32_t>());
            return true;
        }
    };

    template<>
    struct convert<ryu::core::option_list> {
        static Node encode(const ryu::core::option_list& rhs) {
            Node node;

            for (const auto& option : rhs) {
                Node option_node;
                option_node["key"] = option.key;
                option_node["text"] = option.text;
                node.push_back(option_node);
            }

            return node;
        }

        static bool decode(const Node& node, ryu::core::option_list& rhs) {
            if (!node.IsSequence())
                return false;

            for (auto it = node.begin(); it != node.end(); ++it) {
                auto option_node = *it;

                auto key_node = option_node["key"];
                if (key_node.IsNull() || !key_node.IsScalar())
                    continue;

                auto text_node = option_node["text"];
                if (text_node.IsNull() || !text_node.IsScalar())
                    continue;

                rhs.push_back(ryu::core::pick_list_option_t {
                    key_node.as<uint32_t>(),
                    text_node.as<std::string>()});
            }

            return true;
        }
    };

}