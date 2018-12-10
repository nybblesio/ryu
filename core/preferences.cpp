//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <core/yaml_support.h>
#include <core/yaml_converters.h>
#include <common/stream_support.h>
#include "preferences.h"
#include "input_action.h"

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
        emitter << YAML::Key << "rect" << YAML::Flow << YAML::BeginSeq
                << _window_position.left()
                << _window_position.top()
                << _window_position.width()
                << _window_position.height()
                << YAML::EndSeq;
        emitter << YAML::Key << "full_screen" << YAML::Value << _full_screen;
        emitter << YAML::Key << "ide_window_size" << YAML::Value << _ide_window_size;
        emitter << YAML::Key << "emulator_window_size" << YAML::Value << _emulator_window_size;
        emitter << YAML::EndMap;

        emitter << YAML::Key << "actions" << YAML::BeginSeq;

        if (!core::input_action::save(result, emitter))
            return !result.is_failed();

        emitter << YAML::EndSeq;

        ryu::write_text(result, preferences_file_path(), emitter.c_str());

        return !result.is_failed();
    }

    bool preferences::load(core::result& result) {
        auto file_path = preferences_file_path();

        // N.B. it's valid to not have a pre-existing file
        if (!boost::filesystem::exists(file_path))
            return true;

        auto root = YAML::LoadFile(file_path.string());

        std::string default_path_value;

        if (get_optional(root["default_path"], default_path_value))
            default_path(default_path_value);

        auto fonts_node = root["fonts"];
        if (fonts_node != nullptr) {
            std::string font_book_path;
            if (get_optional(fonts_node["book_path"], font_book_path))
                _font_book_path = font_book_path;

            auto ide_node = fonts_node["ide"];
            if (ide_node != nullptr)
                get_font_family_and_size(ide_node, _ide_font);

            auto emulator_node = fonts_node["emulator"];
            if (emulator_node != nullptr)
                get_font_family_and_size(emulator_node, _emulator_font);

            auto engine_node = fonts_node["engine"];
            if (engine_node != nullptr)
                get_font_family_and_size(engine_node, _engine_font);
        }

        auto window_node = root["window"];
        if (window_node != nullptr) {
            _window_position = window_node["rect"].as<core::rect>();

            get_optional(window_node["full_screen"], _full_screen);
            get_optional(window_node["ide_window_size"], _ide_window_size);
            get_optional(window_node["emulator_window_size"], _emulator_window_size);
        }

        auto actions = root["actions"];
        if (actions != nullptr && actions.IsSequence()) {
            core::input_action::load(result, actions);
        }

        return !result.is_failed();
    }

    fs::path preferences::font_book_path() const {
        fs::path temp(_executable_path);
        temp.append(_font_book_path.string());
        return temp;
    }

    fs::path preferences::executable_path() const {
        return _executable_path;
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
            return {0, 0, 1280, 1024};
        return _window_position;
    }

    fs::path preferences::preferences_file_path() const {
        fs::path preferences_path(_executable_path.string());
        preferences_path.append("preferences.yaml");
        return preferences_path;
    }

    void preferences::default_path(const fs::path& value) {
        _default_path = value;
        if (boost::filesystem::exists(_default_path)) {
            boost::filesystem::current_path(_default_path);
        }
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

    void preferences::executable_path(const fs::path& value) {
        _executable_path = value;
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

    core::context_window::sizes preferences::ide_window_size() const {
        return _ide_window_size;
    }

    void preferences::ide_window_size(core::context_window::sizes size) {
        _ide_window_size = size;
    }

    core::context_window::sizes preferences::emulator_window_size() const {
        return _emulator_window_size;
    }

    void preferences::emulator_window_size(core::context_window::sizes size) {
        _emulator_window_size = size;
    }

    bool preferences::get_font_family_and_size(YAML::Node& node, font_value_t& value) {
        std::string family_name;
        uint32_t font_size;

        if (get_optional(node["family_name"], family_name)) {
            if (get_optional(node["size"], font_size)) {
                value.first = family_name;
                value.second = font_size;
                return true;
            }
        }

        return false;
    }

}