//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <fstream>
#include <yaml-cpp/yaml.h>
#include <boost/filesystem/operations.hpp>
#include "project.h"

namespace ryu::core {

    core::project_shared_ptr core::project::_instance = nullptr;
    std::vector<core::project::project_changed_callable> core::project::_listeners;

    bool project::create(
            core::result& result,
            const fs::path& path) {
        fs::path project_path = path;

        if (!project_path.is_absolute()) {
            project_path = fs::current_path().append(project_path.string());
        }

        if (fs::is_directory(project_path)) {
            result.add_message(
                    "C031",
                    fmt::format("project directory already exists: {}", project_path.string()),
                    true);
            return false;
        }

        boost::system::error_code ec;
        if (!fs::create_directory(project_path, ec)) {
            result.add_message(
                    "C031",
                    fmt::format("unable to create directory: {}", ec.message()),
                    true);
            return false;
        }

        fs::path metadata_path(project_path);
        metadata_path.append(".ryu");
        if (!fs::create_directory(metadata_path, ec)) {
            result.add_message(
                    "C031",
                    fmt::format("unable to create metadata directory: {}", ec.message()),
                    true);
            return false;
        }

        _instance = core::project_shared_ptr(new core::project(project_path));
        _instance->save(result);
        notify_listeners();
        return !result.is_failed();
    }

    bool project::load(
            core::result& result,
            const fs::path& path) {
        return false;
    }

    bool project::clone(
            core::result& result,
            const fs::path& source_path,
            const fs::path& target_path) {
        return false;
    }

    void project::notify_listeners() {
        for (const auto& listener : _listeners)
            listener();
    }

    core::project* project::instance() {
        return _instance.get();
    }

    project::project(const fs::path& project_path) : _path(project_path),
                                                     _name(project_path.filename().string()) {
    }

    fs::path project::path() const {
        return _path;
    }

    std::string project::name() const {
        return _name;
    }

    hardware::machine* project::machine() {
        return _machine;
    }

    bool project::save(core::result& result) {
        YAML::Emitter emitter;

        emitter << YAML::BeginMap;
        emitter << YAML::Key << "name" << YAML::Value << _name;
        emitter << YAML::Key << "description" << YAML::Value << _description;

        auto machine_id = _machine != nullptr ? _machine->id() : 0;
        emitter << YAML::Key << "machine" << YAML::Value << machine_id;

        emitter << YAML::Key << "files" << YAML::BeginSeq;
        // XXX: write out project file list
        emitter << YAML::EndSeq;

        emitter << YAML::Key << "environments" << YAML::BeginSeq;
        // XXX: write out environment file list
        emitter << YAML::EndSeq;

        try {
            fs::path project_file(_path);
            project_file
                    .append(".ryu")
                    .append(fmt::format("{}.rproj", _name));

            std::ofstream file;
            file.open(project_file.string());
            file << emitter.c_str();
            file.close();
        } catch (std::exception& e) {
            result.add_message(
                    "C031",
                    fmt::format("unable to save project: {}", e.what()),
                    true);
        }

        return true;
    }

    std::string project::description() const {
        return _description;
    }

    void project::machine(hardware::machine* machine) {
        _machine = machine;
    }

    void project::description(const std::string& value) {
        _description = value;
    }

    void project::add_listener(const project::project_changed_callable& callable) {
        _listeners.push_back(callable);
    }

}