//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <fmt/format.h>
#include <core/id_pool.h>
#include <yaml-cpp/yaml.h>
#include "machine.h"
#include "registry.h"

namespace ryu::hardware {

    registry* registry::instance() {
        static registry instance;
        return &instance;
    }

    bool registry::remove_machine(
            core::result& result,
            const std::string& name) {
        auto machine = find_machine(name);
        if (machine != nullptr) {
            return remove_machine(result, machine->id());
        }
        result.add_message(
                "R008",
                "No machine exists with specified name.",
                true);
        return !result.is_failed();
    }

    bool registry::remove_machine(
            core::result& result,
            uint32_t id) {
        _machines.erase(id);
        // XXX: save the registry file
        return true;
    }

    machine_list registry::machines() {
        machine_list list;
        for (auto& it : _machines)
            list.push_back(&it.second);
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

    display* registry::find_display(uint32_t id) {
        auto& displays = display::catalog();
        for (size_t i = 0; i < displays.size(); i++) {
            if (displays[i].id() == id) {
                return &displays[i];
            }
        }
        return nullptr;
    }

    hardware::machine* registry::find_machine(uint32_t id) {
        auto it = _machines.find(id);
        if (it == _machines.end())
            return nullptr;
        return &it->second;
    }

    ic_type_list registry::integrated_circuit_types() const {
        ic_type_list types;

        auto base = rttr::type::get<ryu::hardware::integrated_circuit>();
        auto component_types = base.get_derived_classes();
        for(const auto& type : component_types) {
            auto ic_type_id = type
                .get_metadata(ryu::hardware::meta_data_key::type_id)
                .convert<uint16_t>();
            auto ic_type_name = type
                .get_metadata(ryu::hardware::meta_data_key::type_name)
                .convert<std::string>();
            types.push_back(integrated_circuit_type_t {ic_type_id, ic_type_name});
        }

        return types;
    }

    bool registry::load(core::result& result, const fs::path& path) {
        _machines.clear();

        if (!boost::filesystem::exists(path))
            return true;

        auto root = YAML::LoadFile(path.string());

        auto machines = root["machines"];
        if (machines != nullptr && machines.IsSequence()) {
            for (auto it = machines.begin(); it != machines.end(); ++it) {
                auto node = *it;
                if (!node.IsMap())
                    continue;

                if (node["id"] == nullptr) {
                    result.add_message(
                            "R004",
                            "Machine node requires id.",
                            true);
                    result.fail();
                    break;
                }

                if (node["name"] == nullptr) {
                    result.add_message(
                            "R004",
                            "Machine node requires name.",
                            true);
                    result.fail();
                    break;
                }

                auto id = node["id"].as<uint32_t>();
                auto name = node["name"].as<std::string>();
                auto machine = hardware::machine(id);
                machine.name(name);

                if (node["address_space"] != nullptr) {
                    auto address_space = node["address_space"].as<uint32_t>();
                    machine.address_space(address_space);
                }

                if (node["display_id"] != nullptr) {
                    auto display_id = node["display_id"].as<uint32_t>();
                    machine.display(find_display(display_id));
                }

                if (node["description"] != nullptr) {
                    auto description = node["description"].as<std::string>();
                    machine.description(description);
                }

                auto components = node["components"];
                if (components != nullptr && components.IsSequence()) {
                    for (auto cit = components.begin(); cit != components.end(); ++cit) {
                        auto component_node = *cit;
                        if (!component_node.IsMap())
                            continue;

                        if (component_node["id"] == nullptr) {
                            result.add_message(
                                    "R004",
                                    "Machine component node requires id.",
                                    true);
                            result.fail();
                            break;
                        }

                        if (component_node["name"] == nullptr) {
                            result.add_message(
                                    "R004",
                                    "Machine component node requires name.",
                                    true);
                            result.fail();
                            break;
                        }

                        if (component_node["address"] == nullptr) {
                            result.add_message(
                                    "R004",
                                    "Machine component node requires address.",
                                    true);
                            result.fail();
                            break;
                        }

                        auto component_id = component_node["id"].as<uint32_t>();
                        auto component_name = component_node["name"].as<std::string>();
                        auto component_address = component_node["address"].as<uint32_t>();

                        auto component_color = 0;
                        auto component_color_node = component_node["color"];
                        if (component_color_node != nullptr) {
                            component_color = component_color_node.as<uint32_t>();
                        }

                        if (component_node["ic"] != nullptr) {
                            auto ic_node = component_node["ic"];
                            if (ic_node.IsMap()) {
                                auto type_id = ic_node["type_id"].as<uint16_t>();

                                auto ic = new_ic_by_type_id(type_id);
                                if (ic == nullptr) {
                                    result.add_message(
                                            "R005",
                                            "No integrated_circuit exists for type_id.",
                                            true);
                                    result.fail();
                                    break;
                                }

                                // XXX: at some point this should be required
                                if (ic_node["address_space"] != nullptr) {
                                    auto ic_address_space = ic_node["address_space"].as<uint32_t>();
                                    ic->address_space(ic_address_space);
                                }

                                auto component = new hardware::component(component_id, ic);
                                component->name(component_name);
                                component->address(component_address);
                                component->color(static_cast<core::palette_index>(component_color));

                                if (component_node["description"] != nullptr) {
                                    auto component_desc = component_node["description"].as<std::string>();
                                    component->description(component_desc);
                                }

                                machine.add_component(component);
                            }
                        }
                    }
                }

                if (!result.is_failed())
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

    hardware::integrated_circuit* registry::new_ic_by_type_id(uint32_t type_id) {
        auto base_type = rttr::type::get<ryu::hardware::integrated_circuit>();
        auto ic_types = base_type.get_derived_classes();
        for(const auto& type : ic_types) {
            auto ic_type_id = type
                    .get_metadata(ryu::hardware::meta_data_key::type_id)
                    .to_int();
            if (type_id == ic_type_id) {
                auto instance = type.create({});
                return instance.get_value<hardware::integrated_circuit*>();
            }
        }
        return nullptr;
    }

}