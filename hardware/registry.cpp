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
#include <core/yaml_support.h>
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

                uint32_t machine_id;
                if (!ryu::core::get_optional(node["id"], machine_id)) {
                    result.add_message(
                        "R004",
                        "Machine node requires id.",
                        true);
                    break;
                }

                std::string machine_name;
                if (!ryu::core::get_optional(node["name"], machine_name)) {
                    result.add_message(
                            "R004",
                            "Machine node requires name.",
                            true);
                    break;
                }

                auto machine = hardware::machine(machine_id);
                machine.name(machine_name);

                uint32_t machine_address_space;
                uint32_t machine_display_id;
                std::string machine_description;

                if (ryu::core::get_optional(node["address_space"], machine_address_space))
                    machine.address_space(machine_address_space);

                if (ryu::core::get_optional(node["display_id"], machine_display_id))
                    machine.display(find_display(machine_display_id));

                if (ryu::core::get_optional(node["description"], machine_description))
                    machine.description(machine_description);

                auto components = node["components"];
                if (components != nullptr && components.IsSequence()) {
                    for (auto cit = components.begin(); cit != components.end(); ++cit) {
                        auto component_node = *cit;
                        if (!component_node.IsMap())
                            continue;

                        uint32_t component_id;
                        uint32_t component_address;
                        std::string component_name;

                        if (!ryu::core::get_optional(component_node["id"], component_id)) {
                            result.add_message(
                                    "R004",
                                    "Machine component node requires id.",
                                    true);
                            break;
                        }

                        if (!ryu::core::get_optional(component_node["name"], component_name)) {
                            result.add_message(
                                    "R004",
                                    "Machine component node requires name.",
                                    true);
                            break;
                        }

                        if (!ryu::core::get_optional(component_node["address"], component_address)) {
                            result.add_message(
                                    "R004",
                                    "Machine component node requires address.",
                                    true);
                            break;
                        }

                        auto component_color = 0;
                        ryu::core::get_optional(component_node["color"], component_color);

                        if (component_node["ic"] != nullptr) {
                            auto ic_node = component_node["ic"];
                            if (ic_node.IsMap()) {
                                uint16_t type_id;
                                ryu::core::get_optional(ic_node["type_id"], type_id);

                                auto ic = new_ic_by_type_id(type_id);
                                if (ic == nullptr) {
                                    result.add_message(
                                            "R005",
                                            "No integrated_circuit exists for type_id.",
                                            true);
                                    break;
                                }

                                uint32_t ic_address_space;
                                if (!ryu::core::get_optional(ic_node["address_space"], ic_address_space)) {
                                    result.add_message(
                                        "R005",
                                        "address_space is required for integrated_circuit.",
                                        true);
                                    break;
                                }

                                ic->address_space(ic_address_space);

                                auto writes_node = ic_node["writes"];
                                if (writes_node != nullptr && writes_node.IsSequence()) {
                                    for (auto wit = writes_node.begin();
                                         wit != writes_node.end();
                                         ++wit) {
                                        auto write_node = *wit;
                                        if (!write_node.IsMap())
                                            continue;

                                        auto offset_node = write_node["offset"];
                                        auto size_node = write_node["size"];
                                        auto value_node = write_node["value"];

                                        if (offset_node == nullptr || value_node == nullptr)
                                            continue;

                                        uint32_t size = 1;
                                        if (size_node != nullptr && size_node.IsScalar())
                                            size = size_node.as<uint32_t>();

                                        auto offset = offset_node.as<uint32_t>();
                                        auto value = value_node.as<uint32_t>();

                                        switch (size) {
                                            case 1:
                                                ic->write_byte(offset, static_cast<uint8_t>(value));
                                                break;
                                            case 2:
                                                ic->write_word(
                                                    offset,
                                                    static_cast<uint16_t>(value),
                                                    integrated_circuit::endianness::big);
                                                break;
                                            case 4:
                                                ic->write_dword(
                                                    offset,
                                                    value,
                                                    integrated_circuit::endianness::big);
                                                break;
                                            default: {
                                                result.add_message(
                                                    "R008",
                                                    fmt::format("Unknown write size: {}", size),
                                                    true);
                                                break;
                                            }
                                        }
                                    }
                                }

                                auto component = new hardware::component(component_id, ic);
                                component->name(component_name);
                                component->address(component_address);
                                component->color(static_cast<core::palette_index>(component_color));

                                std::string component_description;
                                if (ryu::core::get_optional(component_node["description"], component_description))
                                    component->description(component_description);

                                machine.add_component(component);
                            }
                        }
                    }
                }

                if (!result.is_failed())
                    _machines.insert(std::make_pair(machine_id, machine));
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
                    .to_uint32();
            if (type_id == ic_type_id) {
                auto instance = type.create({});
                auto circuit = instance.get_value<hardware::integrated_circuit*>();
                circuit->initialize();
                return circuit;
            }
        }
        return nullptr;
    }

}