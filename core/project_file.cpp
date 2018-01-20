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
        auto id = node["id"];
        if (id == nullptr) {
            result.add_message(
                    "C031",
                    "project_file id is a required value",
                    true);
            return {};
        }

        auto path = node["path"];
        if (path == nullptr) {
            result.add_message(
                    "C031",
                    "project_file path is a required value",
                    true);
            return {};
        }

        auto type = node["type"];
        if (type == nullptr) {
            result.add_message(
                    "C031",
                    "project_file type is a required value",
                    true);
            return {};
        }

        project_file file(
                id.as<uint32_t>(),
                fs::path(path.as<std::string>()),
                string_to_type(type.as<std::string>()));

        auto cpu_component_id = node["cpu_component_id"].as<uint32_t>();
        if (cpu_component_id != 0 && machine != nullptr) {
            auto component = machine->find_component(cpu_component_id);
            if (component == nullptr) {
                result.add_message(
                        "C031",
                        "project_file cpu not found on machine",
                        true);
                return {};
            }
            file.cpu(component);
        }

        return file;
    }

    project_file::project_file(
            uint32_t id,
            const fs::path& path,
            project_file::types type) : _id(id),
                                        _path(path),
                                        _type(type) {
    }

    bool project_file::dirty() const {
        return _dirty;
    }

    uint32_t project_file::id() const {
        return _id;
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

    project_file::types project_file::type() const {
        return _type;
    }

    void project_file::cpu(hardware::component* value) {
        _cpu = value;
    }

    void project_file::type(project_file::types value) {
        _type = value;
    }

    std::string project_file::type_to_string(project_file::types type) {
        switch (type) {
            case assembly_source: return "assembly_source";
            case environment:     return "environment";
            default:              return "uninitialized";
        }
    }

    bool project_file::save(core::result& result, YAML::Emitter& emitter) {
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "id" << YAML::Value << _id;
        emitter << YAML::Key << "path" << YAML::Value << _path.string();
        emitter << YAML::Key << "type" << YAML::Value << type_to_string(_type);
        if (_cpu != nullptr)
            emitter << YAML::Key << "cpu_component_id" << YAML::Value << _cpu->id();
        emitter << YAML::EndMap;
        return true;
    }

    project_file::types project_file::string_to_type(const std::string& name) {
        if (name == "assembly_source") {
            return project_file::types::assembly_source;
        } else if (name == "environment") {
            return project_file::types::environment;
        } else {
            return project_file::types::uninitialized;
        }
    }

}