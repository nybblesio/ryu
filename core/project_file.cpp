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
                project_file_type::code_to_type(type.as<std::string>()));

        return file;
    }

    project_file::project_file(
            uint32_t id,
            const fs::path& path,
            project_file_type::codes type) : _id(id),
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

    project_file_type::codes project_file::type() const {
        return _type;
    }

    void project_file::type(project_file_type::codes value) {
        _type = value;
    }

    bool project_file::save(core::result& result, YAML::Emitter& emitter) {
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "id" << YAML::Value << _id;
        emitter << YAML::Key << "path" << YAML::Value << _path.string();
        emitter << YAML::Key << "type" << YAML::Value << project_file_type::type_to_code(_type);
        emitter << YAML::EndMap;
        return true;
    }

}