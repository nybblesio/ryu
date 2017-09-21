//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <core/id_pool.h>
#include <yaml-cpp/yaml.h>
#include <fmt/format.h>
#include "machine.h"
#include "registry.h"

namespace ryu::hardware {

    registry* registry::instance() {
        static registry instance;
        return &instance;
    }

    machine_list registry::machines() {
        machine_list list;
        for (auto& it : _machines)
            list.push_back(&it.second);
        return list;
    }

    void registry::remove_machine(int id) {
        _machines.erase(id);
    }

    hardware::machine* registry::new_machine() {
        auto id = core::id_pool::instance()->allocate();
        auto it = _machines.insert(std::make_pair(id, hardware::machine(id)));
        auto* instance = &(it.first)->second;
        instance->name(fmt::format("machine {}", id));
        instance->initialize();
        return instance;
    }

    hardware::machine* registry::find_machine(int id) {
        auto it = _machines.find(id);
        if (it == _machines.end())
            return nullptr;
        return &it->second;
    }

    bool registry::load(core::result& result, const fs::path& path) {
        return false;
    }

    bool registry::save(core::result& result, const fs::path& path) {
        YAML::Emitter emitter;
        return false;
    }

    hardware::machine* registry::find_machine(const std::string& name) {
        for (auto it = _machines.begin(); it != _machines.end(); ++it) {
            if ((*it).second.name() == name) {
                return &(*it).second;
            }
        }
        return nullptr;
    }

}