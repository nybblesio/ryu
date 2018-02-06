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
#include <hardware/registry.h>
#include <boost/filesystem/operations.hpp>
#include "project.h"
#include "project_file.h"

namespace ryu::core {

    bool core::project::_notify_enabled = true;
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

        return !result.is_failed();
    }

    // XXX: would be nice to add feature to scan up parent directories
    //      to see if .ryu/arcade.rproj is found.
    bool project::load(
            core::result& result,
            const fs::path& path) {
        fs::path project_path = path;

        if (!project_path.is_absolute()) {
            project_path = fs::current_path().append(project_path.string());
        }

        if (!fs::exists(project_path)) {
            result.add_message(
                    "C031",
                    fmt::format("project does not exist: {}", project_path.string()),
                    true);
            return false;
        }

        fs::path project_file(path);
        project_file
                .append(".ryu")
                .append("arcade.rproj");

        if (!fs::exists(project_file)) {
            result.add_message(
                    "C031",
                    fmt::format("project file does not exist: {}", project_file.string()),
                    true);
            return false;
        }

        suspend_notify();

        auto root = YAML::LoadFile(project_file.string());

        if (root["name"] == nullptr) {
            result.add_message(
                    "C031",
                    "Project requires name.",
                    true);
            result.fail();
            return false;
        }

        auto name = root["name"];
        _instance = core::project_shared_ptr(new core::project(path));
        _instance->name(name.as<std::string>());

        auto description = root["description"];
        if (description != nullptr) {
            _instance->description(description.as<std::string>());
        }

        hardware::machine* machine = nullptr;
        auto machine_node = root["machine"];
        if (machine_node != nullptr && machine_node.IsScalar()) {
            auto machine_id = machine_node.as<uint32_t>();
            if (machine_id != 0) {
                machine = hardware::registry::instance()->find_machine(machine_id);
                if (machine == nullptr) {
                    result.add_message(
                            "C031",
                            fmt::format("no machine exists with id: {}", machine_id),
                            true);
                    return false;
                }
                _instance->machine(machine);
            }
        }

        auto files = root["files"];
        if (files != nullptr && files.IsSequence()) {
            for (auto it = files.begin(); it != files.end(); ++it) {
                auto file_node = *it;
                auto file = core::project_file::load(result, file_node);
                if (file.type() != core::project_file_type::uninitialized) {
                    _instance->add_file(file);
                }
            }
        }

        auto active_environment_node = root["active_environment"];
        if (active_environment_node != nullptr && active_environment_node.IsScalar()) {
            auto active_environment_id = active_environment_node.as<uint32_t>();
            if (active_environment_id != 0) {
                auto file = _instance->find_file(active_environment_id);
                if (file == nullptr) {
                    result.add_message(
                            "C031",
                            fmt::format("no project_file exists with id: {}", active_environment_id),
                            true);
                    return false;
                }
                _instance->active_environment(file);
            }
        }

        auto props = root["props"];
        if (props != nullptr && props.IsSequence()) {
            for (auto it = props.begin(); it != props.end(); ++it) {
                auto prop_node = *it;
                _instance->prop(
                        prop_node["key"].as<std::string>(),
                        prop_node["value"].as<std::string>());
            }
        }

        _instance->_dirty = false;

        resume_notify();

        return true;
    }

    bool project::clone(
            core::result& result,
            const fs::path& source_path,
            const fs::path& target_path) {
        if (_instance == nullptr) {
            result.add_message(
                    "C032",
                    "no project is loaded; clone failed",
                    true);
            return false;
        }

        return false;
    }

    bool project::close(core::result& result) {
        if (_instance == nullptr) {
            result.add_message(
                    "C032",
                    "no project is loaded; close failed",
                    true);
            return false;
        }

        _instance = nullptr;
        notify_listeners();

        return true;
    }

    fs::path project::find_project_root() {
        return fs::current_path();
    }

    void project::resume_notify() {
        _notify_enabled = true;
        notify_listeners();
    }

    void project::suspend_notify() {
        _notify_enabled = false;
    }

    void project::notify_listeners() {
        if (!_notify_enabled)
            return;
        for (const auto& listener : _listeners)
            listener();
    }

    core::project* project::instance() {
        return _instance.get();
    }

    project::project(const fs::path& project_path) : _path(project_path),
                                                     _name(project_path.filename().string()) {
    }

    bool project::dirty() const {
        return _dirty;
    }

    fs::path project::path() const {
        return _path;
    }

    void project::remove_all_files() {
        _files.clear();
        _dirty = true;
        notify_listeners();
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

        auto active_environment_id = _active_environment != nullptr ? _active_environment->id() : 0;
        emitter << YAML::Key << "active_environment" << YAML::Value << active_environment_id;

        emitter << YAML::Key << "files" << YAML::BeginSeq;
        for (auto& file : _files)
            file.save(result, emitter);
        emitter << YAML::EndSeq;

        emitter << YAML::Key << "props" << YAML::BeginSeq;
        for (auto& it : _props) {
            emitter << YAML::BeginMap;
            emitter << YAML::Key << "key" << it.first;
            emitter << YAML::Key << "value" << it.second;
            emitter << YAML::EndMap;
        }
        emitter << YAML::EndSeq;

        try {
            fs::path project_file(_path);
            project_file
                    .append(".ryu")
                    .append("arcade.rproj");

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

        _dirty = false;
        if (!result.is_failed())
            notify_listeners();

        return !result.is_failed();
    }

    void project::remove_file(uint32_t id) {
        for (size_t i = 0; i < _files.size(); i++) {
            auto& file = _files[i];
            if (file.id() == id) {
                _files.erase(_files.begin() + i);
                _dirty = true;
                notify_listeners();
                break;
            }
        }
        std::sort(
                _files.begin(),
                _files.end(),
                [](project_file& left, project_file& right) {
                    return left.sequence() < right.sequence();
                });
    }

    std::string project::description() const {
        return _description;
    }

    project_file* project::active_environment() {
        return _active_environment;
    }

    void project::name(const std::string& value) {
        _name = value;
        _dirty = true;
        notify_listeners();
    }

    project_file* project::find_file(uint32_t id) {
        for (size_t i = 0; i < _files.size(); i++) {
            if (_files[i].id() == id) {
                return &_files[i];
            }
        }
        return nullptr;
    }

    const project_file_list& project::files() const {
        return _files;
    }

    void project::machine(hardware::machine* machine) {
        _machine = machine;
        _dirty = true;
        notify_listeners();
    }

    void project::add_file(const project_file& value) {
        _files.push_back(value);
        std::sort(
                _files.begin(),
                _files.end(),
                [](project_file& left, project_file& right) {
                    return left.sequence() < right.sequence();
                });
        _dirty = true;
        notify_listeners();
    }

    void project::description(const std::string& value) {
        _description = value;
        _dirty = true;
        notify_listeners();
    }

    void project::active_environment(project_file* value) {
        _active_environment = value;
        _dirty = true;
        notify_listeners();
    }

    project_file* project::find_file(const fs::path& path) {
        for (size_t i = 0; i < _files.size(); i++) {
            if (_files[i].path() == path) {
                return &_files[i];
            }
        }
        return nullptr;
    }

    std::string project::prop(const std::string& key) const {
        auto it = _props.find(key);
        if (it != _props.end())
            return it->second;
        return "";
    }

    void project::prop(const std::string& key, const std::string& value) {
        _props[key] = value;
        _dirty = true;
        notify_listeners();
    }

    void project::add_listener(const project::project_changed_callable& callable) {
        _listeners.push_back(callable);
    }

}