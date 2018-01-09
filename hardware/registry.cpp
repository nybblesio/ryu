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

    display* registry::find_display(int id) {
        auto& displays = display::catalog();
        for (size_t i = 0; i < displays.size(); i++) {
            if (displays[i].id() == id) {
                return &displays[i];
            }
        }
        return nullptr;
    }

    ic_list registry::integrated_circuits() {
        ic_list list;
        // XXX: use RTTR to reflect the list of integrated_circuits
        return list;
    }

    display_list& registry::displays() const {
        return display::catalog();
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

    hardware::integrated_circuit* registry::find_ic(int id) {
        return nullptr;
    }

    bool registry::load(core::result& result, const fs::path& path) {
        _machines.clear();

        auto root = YAML::LoadFile(path.string());

        auto machines = root["machines"];
        if (machines != nullptr && machines.IsSequence()) {
            for (auto it = machines.begin(); it != machines.end(); ++it) {
                auto node = *it;
                if (!node.IsMap())
                    continue;

                if (node["id"] == nullptr) {
                    result.add_message("R004", "Machine node requires id.", true);
                    result.fail();
                    break;
                }

                if (node["name"] == nullptr) {
                    result.add_message("R004", "Machine node requires name.", true);
                    result.fail();
                    break;
                }

                auto id = node["id"].as<int>();
                auto name = node["name"].as<std::string>();
                auto machine = hardware::machine(id);
                machine.name(name);

                if (node["address_space"] != nullptr) {
                    auto address_space = node["address_space"].as<int>();
                    machine.address_space(address_space);
                }

                if (node["display_id"] != nullptr) {
                    auto display_id = node["display_id"].as<int>();
                    machine.display(find_display(display_id));
                }

                auto components = node["components"];
                if (components != nullptr && components.IsSequence()) {
                    for (auto cit = components.begin(); cit != components.end(); ++cit) {
                        auto component_node = *cit;
                        if (!component_node.IsMap())
                            continue;
                        auto component_id = component_node["id"].as<int>();
                    }
                }

                _machines.insert(std::make_pair(id, machine));
            }
        }

        return !result.is_failed();
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