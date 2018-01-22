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
#include "preferences.h"

namespace ryu::core {

    bool preferences::full_screen() const {
        return _full_screen;
    }

    void preferences::full_screen(bool value) {
        _full_screen = value;
    }

    fs::path preferences::default_path() const {
        return _default_path;
    }

    font_value_t preferences::ide_font() const {
        return _ide_font;
    }

    bool preferences::save(core::result& result) {
        YAML::Emitter emitter;

        emitter << YAML::BeginMap;

        emitter << YAML::Key << "fonts" << YAML::BeginMap;
        emitter << YAML::Key << "book_path" << YAML::Value << _font_book_path.string();

        emitter << YAML::Key << "ide" << YAML::BeginMap;
        emitter << YAML::Key << "family_name" << YAML::Value << _ide_font.first;
        emitter << YAML::Key << "size" << YAML::Value << _ide_font.second;
        emitter << YAML::EndMap;

        emitter << YAML::Key << "emulator" << YAML::BeginMap;
        emitter << YAML::Key << "family_name" << YAML::Value << _emulator_font.first;
        emitter << YAML::Key << "size" << YAML::Value << _emulator_font.second;
        emitter << YAML::EndMap;

        emitter << YAML::Key << "engine" << YAML::BeginMap;
        emitter << YAML::Key << "family_name" << YAML::Value << _engine_font.first;
        emitter << YAML::Key << "size" << YAML::Value << _engine_font.second;
        emitter << YAML::EndMap;
        emitter << YAML::EndMap;

        emitter << YAML::Key << "default_path" << YAML::Value << _default_path.string();

        emitter << YAML::Key << "window" << YAML::BeginMap;
        emitter << YAML::Key << "left" << YAML::Value << _window_position.left();
        emitter << YAML::Key << "top" << YAML::Value << _window_position.top();
        emitter << YAML::Key << "width" << YAML::Value << _window_position.width();
        emitter << YAML::Key << "height" << YAML::Value << _window_position.height();
        emitter << YAML::Key << "full_screen" << YAML::Value << _full_screen;
        emitter << YAML::EndMap;

        emitter << YAML::EndMap;

        try {
            std::ofstream file;
            file.open("preferences.yaml");
            file << emitter.c_str();
            file.close();
        } catch (std::exception& e) {
            result.add_message(
                    "P001",
                    fmt::format("unable to preferences: {}", e.what()),
                    true);
        }

        return !result.is_failed();
    }

    bool preferences::load(core::result& result) {
        // N.B. it's valid to not have a pre-existing file
        if (!boost::filesystem::exists("preferences.yaml"))
            return true;

        auto root = YAML::LoadFile("preferences.yaml");

        auto default_path_node = root["default_path"];
        if (default_path_node != nullptr) {
            _default_path = default_path_node.as<std::string>();
        }

        auto fonts_node = root["fonts"];
        if (fonts_node != nullptr) {
            auto font_book_path_node = fonts_node["book_path"];
            if (font_book_path_node != nullptr) {
                _font_book_path = font_book_path_node.as<std::string>();
            }

            auto ide_node = fonts_node["ide"];
            if (ide_node != nullptr) {
                auto family_name_node = ide_node["family_name"];
                if (family_name_node != nullptr) {
                    auto size_node = ide_node["size"];
                    if (size_node != nullptr) {
                        _ide_font = std::make_pair(
                                family_name_node.as<std::string>(),
                                size_node.as<uint32_t>());
                    }
                }
            }

            auto emulator_node = fonts_node["emulator"];
            if (emulator_node != nullptr) {
                auto family_name_node = emulator_node["family_name"];
                if (family_name_node != nullptr) {
                    auto size_node = emulator_node["size"];
                    if (size_node != nullptr) {
                        _emulator_font = std::make_pair(
                                family_name_node.as<std::string>(),
                                size_node.as<uint32_t>());
                    }
                }
            }

            auto engine_node = fonts_node["engine"];
            if (engine_node != nullptr) {
                auto family_name_node = engine_node["family_name"];
                if (family_name_node != nullptr) {
                    auto size_node = engine_node["size"];
                    if (size_node != nullptr) {
                        _engine_font = std::make_pair(
                                family_name_node.as<std::string>(),
                                size_node.as<uint32_t>());
                    }
                }
            }
        }

        auto window_node = root["window"];
        if (window_node != nullptr) {
            int32_t left = 0, top = 0, width = 0, height = 0;

            auto left_node = window_node["left"];
            if (left_node != nullptr) {
                left = left_node.as<int32_t>();
            }

            auto top_node = window_node["top"];
            if (top_node != nullptr) {
                top = top_node.as<int32_t>();
            }

            auto width_node = window_node["width"];
            if (width_node != nullptr) {
                width = width_node.as<int32_t>();
            }

            auto height_node = window_node["height"];
            if (height_node != nullptr) {
                height = height_node.as<int32_t>();
            }

            _window_position = {left, top, width, height};

            auto full_screen_node = window_node["full_screen"];
            if (full_screen_node != nullptr) {
                _full_screen = full_screen_node.as<bool>();
            }
        }

        return !result.is_failed();
    }

    fs::path preferences::font_book_path() const {
        return _font_book_path;
    }

    font_value_t preferences::engine_font() const {
        return _engine_font;
    }

    core::palette preferences::ide_palette() const {
        return _ide_palette;
    }

    font_value_t preferences::emulator_font() const {
        return _emulator_font;
    }

    core::rect preferences::window_position() const {
        if (_window_position.empty())
            return {
                    SDL_WINDOWPOS_CENTERED,
                    SDL_WINDOWPOS_CENTERED,
                    1920,
                    1080
            };
        return _window_position;
    }

    void preferences::default_path(const fs::path& value) {
        _default_path = value;
    }

    void preferences::ide_font(const font_value_t& value) {
        _ide_font = value;
    }

    void preferences::font_book_path(const fs::path& value) {
        _font_book_path = value;
    }

    void preferences::engine_font(const font_value_t& value) {
        _engine_font = value;
    }

    void preferences::ide_palette(const core::palette& value) {
        _ide_palette = value;
    }

    void preferences::window_position(const core::rect& value) {
        _window_position = value;
    }

    void preferences::emulator_font(const font_value_t& value) {
        _emulator_font = value;
    }

}