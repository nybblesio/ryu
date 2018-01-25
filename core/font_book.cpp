//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <yaml-cpp/yaml.h>
#include <boost/filesystem/operations.hpp>
#include "font_book.h"

namespace ryu::core {

    font_book* font_book::instance() {
        static font_book instance;
        return &instance;
    }

    bool font_book::load(
            core::result& result,
            const fs::path& path) {
        if (!fs::exists(path)) {
            result.add_message(
                    "F001",
                    fmt::format("font book not exist: {}", path.string()),
                    true);
            return false;
        }

        auto root = YAML::LoadFile(path.string());

        auto fonts_node = root["fonts"];
        if (fonts_node == nullptr || !fonts_node.IsSequence()) {
            result.add_message(
                    "F001",
                    "font book is missing fonts node",
                    true);
            return false;
        }

        for (auto it = fonts_node.begin();
             it != fonts_node.end();
             ++it) {
            auto family_node = *it;
            if (!family_node.IsMap())
                continue;

            std::string family_name;
            std::vector<uint32_t> valid_sizes {};

            auto family_name_node = family_node["family"];
            if (family_name_node != nullptr
            &&  family_name_node.IsScalar()) {
                family_name = family_name_node.as<std::string>();
            }

            auto sizes_node = family_node["sizes"];
            if (sizes_node != nullptr
            &&  sizes_node.IsSequence()) {
                valid_sizes = sizes_node.as<std::vector<uint32_t>>();
            }

            for (size_t i = 0; i < valid_sizes.size(); i++) {
                auto current_size = valid_sizes[i];
                auto current_family = add_font_family(
                        family_name,
                        current_size);

                auto styles_node = family_node["styles"];
                if (styles_node.IsSequence()) {
                    for (auto style_it = styles_node.begin();
                         style_it != styles_node.end();
                         ++style_it) {
                        auto style_node = *style_it;

                        uint8_t font_flags = font::styles::normal;
                        fs::path font_file;

                        auto flags_node = style_node["flags"];
                        if (flags_node != nullptr && flags_node.IsScalar()) {
                            font_flags = font::string_to_style(flags_node.as<std::string>());
                        }

                        auto font_file_node = style_node["font_file"];
                        if (font_file_node != nullptr && font_file_node.IsScalar()) {
                            font_file = font_file_node.as<std::string>();
                        }

                        fs::path full_font_file_path(path.parent_path());
                        full_font_file_path.append(font_file.string());

                        current_family->add_style(font_flags, full_font_file_path);
                    }
                }
            }
        }

        return true;
    }

    core::font_family* font_book::add_font_family(
            const std::string& name,
            uint32_t size) {
        auto it = _font_families.insert(std::make_pair(
                fmt::format("{}:{}", name, size),
                font_family(name, size, _renderer)));
        return &(*it.first).second;
    }

    core::font_family* font_book::find_font_family(
            const std::string& name,
            uint32_t size) {
        auto it = _font_families.find(fmt::format("{}:{}", name, size));
        if (it == _font_families.end())
            return nullptr;
        return &it->second;
    }

    void font_book::renderer(SDL_Renderer* value) {
        _renderer = value;
    }

}