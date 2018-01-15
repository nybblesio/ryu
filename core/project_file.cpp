//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <hardware/machine.h>
#include "project_file.h"

namespace ryu::core {

    project_file project_file::load(
            core::result& result,
            const hardware::machine* machine,
            YAML::Node& node) {
        auto path = node["path"];
        if (path == nullptr) {
            return {};
        }

        auto type = node["type"];
        if (type == nullptr) {
            return {};
        }

        project_file file(
                fs::path(path.as<std::string>()),
                static_cast<core::project_file_types>(type.as<uint32_t>()));

        auto cpu_component_id = node["cpu_component_id"].as<uint32_t>();
        if (cpu_component_id != 0) {
            auto component = machine->find_component(cpu_component_id);
            if (component == nullptr) {
                return {};
            }
            file.cpu(component);
        }

        return file;
    }

    project_file::project_file(
            const fs::path& path,
            core::project_file_types type) : _path(path),
                                             _type(type) {
    }

    bool project_file::dirty() const {
        return _dirty;
    }

    fs::path project_file::path() const {
        return _path;
    }

    void project_file::dirty(bool value) {
        _dirty = value;
    }

    hardware::component* project_file::cpu() {
        return _cpu;
    }

    void project_file::path(const fs::path& value) {
        _path = value;
    }

    void project_file::cpu(hardware::component* value) {
        _cpu = value;
    }

    core::project_file_types project_file::type() const {
        return _type;
    }

    void project_file::type(core::project_file_types value) {
        _type = value;
    }

    bool project_file::save(core::result& result, YAML::Emitter& emitter) {
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "path" << YAML::Value << _path.string();
        emitter << YAML::Key << "type" << YAML::Value << _type;
        if (_cpu != nullptr)
            emitter << YAML::Key << "cpu_component_id" << YAML::Value << _cpu->id();
        emitter << YAML::EndMap;
        return false;
    }

}