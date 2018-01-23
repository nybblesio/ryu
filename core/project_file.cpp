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

        return file;
    }

    project_file::project_file(
            uint32_t id,
            const fs::path& path,
            project_file::types type) : _id(id),
                                        _path(path),
                                        _type(type) {
    }

    // XXX: consider using ctemplate and assets/templates/*.tmpl
    //      to make this feature nicer
    bool project_file::create_stub_file(
            core::result& result,
            const fs::path& path) {
        fs::path file_path = path;

        if (!file_path.is_absolute()) {
            file_path = fs::current_path().append(file_path.string());
        }

        if (fs::exists(file_path)) {
            return true;
        }

        if (!fs::is_directory(file_path.parent_path())) {
            boost::system::error_code ec;
            if (!fs::create_directories(file_path.parent_path(), ec)) {
                result.add_message(
                    "C031",
                    fmt::format("unable to create directory: {}", ec.message()),
                    true);
                return false;
            }
        }

        std::stringstream stream;
        stream << "*\n";
        stream << "* " << file_path.filename() << "\n";
        stream << "*\n\n";

        try {
            std::ofstream file;
            file.open(file_path.string());
            file << stream.str();
            file.close();
        } catch (std::exception& e) {
            result.add_message(
                "C031",
                fmt::format("unable to create file: {}", e.what()),
                true);
        }

        return !result.is_failed();
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

    void project_file::path(const fs::path& value) {
        _path = value;
    }

    project_file::types project_file::type() const {
        return _type;
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