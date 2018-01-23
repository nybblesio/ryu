//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <algorithm>
#include <hardware/machine.h>
#include <common/string_support.h>
#include <boost/algorithm/string.hpp>
#include "project_file.h"

namespace ryu::core {

    static std::map<uint8_t, std::string> s_type_to_code = {
        {project_file::types::source,      "text"},
        {project_file::types::data,        "data"},
        {project_file::types::tiles,       "tiles"},
        {project_file::types::sprites,     "sprites"},
        {project_file::types::module,      "module"},
        {project_file::types::sample,      "sample"},
        {project_file::types::environment, "environ"},
        {project_file::types::background,  "bg"}
    };

    static std::map<std::string, uint8_t> s_code_to_type = {
        {"text",    project_file::types::source},
        {"data",    project_file::types::data},
        {"tiles",   project_file::types::tiles},
        {"sprites", project_file::types::sprites},
        {"module",  project_file::types::module},
        {"sample",  project_file::types::sample},
        {"bg",      project_file::types::background},
        {"environ", project_file::types::environment}
    };

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
                code_to_type(type.as<std::string>()));

        auto cpu_component_node = node["cpu_component_id"];
        if (cpu_component_node != nullptr && cpu_component_node.IsScalar()) {
            auto cpu_component_id = cpu_component_node.as<uint32_t>();
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

    std::string project_file::type_to_code(project_file::types type) {
        auto it = s_type_to_code.find(type);
        if (it == s_type_to_code.end())
            return "UNKNOWN";
        return it->second;
    }

    bool project_file::save(core::result& result, YAML::Emitter& emitter) {
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "id" << YAML::Value << _id;
        emitter << YAML::Key << "path" << YAML::Value << _path.string();
        emitter << YAML::Key << "type" << YAML::Value << type_to_code(_type);
        if (_cpu != nullptr)
            emitter << YAML::Key << "cpu_component_id" << YAML::Value << _cpu->id();
        emitter << YAML::EndMap;
        return true;
    }

    project_file::types project_file::code_to_type(const std::string& name) {
        auto it = s_code_to_type.find(boost::to_lower_copy<std::string>(name));
        if (it == s_code_to_type.end())
            return project_file::types::uninitialized;

        return static_cast<project_file::types>(it->second);
    }

}