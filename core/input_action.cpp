//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <utility>
#include <logger_factory.h>
#include "id_pool.h"
#include "input_action.h"

namespace ryu::core {

    input_action_catalog input_action::s_catalog {};
    input_action_index input_action::s_catalog_index {};

    static logger* s_log = logger_factory::instance()->create(
        "input_action",
        logger::level::info);

    input_action* input_action::create(
            const std::string& name,
            const std::string& category,
            const std::string& description,
            action_type_flags types,
            action_flags flags) {
        return inner_create(
                id_pool::instance()->allocate(),
                name,
                category,
                description,
                types,
                flags);
    }

    input_action* input_action::inner_create(
            uint32_t id,
            const std::string& name,
            const std::string& category,
            const std::string& description,
            action_type_flags types,
            action_flags flags) {
        auto action = find_by_name(name);
        if (action != nullptr)
            return action;

        if (s_catalog.capacity() < 1024)
            s_catalog.reserve(1024);

        s_catalog.emplace_back(
                id,
                name,
                category,
                description,
                types,
                flags);
        action = &s_catalog.back();
        action_type_flags type = input_action::type::keyboard;
        while (type != input_action::type::last) {
            if ((types & type) == 0) {
                type <<= 1;
                continue;
            }
            auto it = s_catalog_index.find(type);
            if (it == s_catalog_index.end()) {
                s_catalog_index.insert(std::make_pair(
                    type,
                    input_action_ptr_list {action}));
            } else {
                auto& list = it->second;
                list.push_back(action);
            }
            type <<= 1;
        }
        return action;
    }

    input_action* input_action::create_no_map(
            const std::string& name,
            const std::string& category,
            const std::string& description,
            action_type_flags types) {
        return inner_create(
                id_pool::instance()->allocate(),
                name,
                category,
                description,
                types,
                input_action::flag::no_map);
    }

    bool input_action::exists(const std::string& name) {
        return find_by_name(name) != nullptr;
    }

    const input_action_catalog& input_action::catalog() {
        return s_catalog;
    }

    bool input_action::load(core::result& result, YAML::Node& root) {
        for (auto it = root.begin(); it != root.end(); ++it) {
            auto action_node = *it;
            if (!action_node.IsMap())
                continue;

            auto action_id = action_node["id"].as<uint32_t>();
            auto name = action_node["name"].as<std::string>();
            auto category = action_node["category"].as<std::string>();
            auto description = action_node["description"].as<std::string>();
            auto flags = static_cast<action_flags>(action_node["flags"].as<uint32_t>());

            action_type_flags types = input_action::type::keyboard;
            auto types_node = action_node["types"];
            if (!types_node.IsNull() && types_node.IsScalar()) {
                types = static_cast<action_type_flags>(types_node.as<uint32_t>());
            }

            id_pool::instance()->mark_used(action_id);
            auto action = inner_create(
                    action_id,
                    name,
                    category,
                    description,
                    types,
                    flags);

            auto bindings_node = action_node["bindings"];
            if (bindings_node != nullptr && bindings_node.IsSequence()) {
                for (auto binding_it = bindings_node.begin();
                         binding_it != bindings_node.end();
                         ++binding_it) {
                    auto binding_node = *binding_it;
                    if (!binding_node.IsMap())
                        continue;

                    auto type = binding_node["type"].as<std::string>();
                    if (type == "key_combination") {
                        auto keys = binding_node["keys"].as<input_keys>();
                        action->bind_keys(keys);
                    } else if (type == "joystick") {
                        auto joystick_id = binding_node["joystick_id"].as<uint32_t>();
                        auto event_type = binding_node["event_type"].as<std::string>();
                        if (event_type == "hat") {
                            auto id = binding_node["hat_id"].as<uint8_t>();
                            auto state = binding_node["hat_state"].as<hat_state>();
                            action->bind_joystick_hat(joystick_id, id, state);
                        } else if (event_type == "button") {
                            auto state = binding_node["button_state"].as<button_state>();
                            action->bind_joystick_buttons(joystick_id, state);
                        } else if (event_type == "axis") {
                        } else if (event_type == "ball") {
                        }
                    } else if (type == "mouse") {
                    }
                }
            }
        }

        return !result.is_failed();
    }

    bool input_action::save(core::result& result, YAML::Emitter& emitter) {
        for (const auto& action : s_catalog) {
            if (!action.can_map())
                continue;

            emitter << YAML::BeginMap;
            emitter << YAML::Key << "id" << YAML::Value << action._id;
            emitter << YAML::Key << "flags" << YAML::Hex << (int)action._flags;
            emitter << YAML::Key << "types" << YAML::Hex << (int)action._types;
            emitter << YAML::Key << "name" << YAML::Value << action._name;
            emitter << YAML::Key << "category" << YAML::Value << action._category;
            emitter << YAML::Key << "description" << YAML::Value << action._description;
            emitter << YAML::Key << "bindings" << YAML::BeginSeq;
            for (const auto& binding : action._bindings) {
                emitter << YAML::BeginMap;
                switch (binding.type()) {
                    case input_binding::key_combination: {
                        emitter << YAML::Key << "type" << YAML::Value << "key_combination";
                        emitter << YAML::Key << "keys" << YAML::Flow << binding.key_config();
                        break;
                    }
                    case input_binding::joystick: {
                        auto config = binding.joystick_config();
                        emitter << YAML::Key << "type" << YAML::Value << "joystick";
                        emitter << YAML::Key << "joystick_id" << config->_index;
                        switch (config->_type) {
                            case input_joystick_t::hat:
                                emitter << YAML::Key << "event_type" << YAML::Value << "hat";
                                emitter << YAML::Key << "hat_id" << YAML::Value << config->_hat_id;
                                emitter << YAML::Key << "hat_state" << YAML::Value << config->_hat_state;
                                break;
                            case input_joystick_t::button:
                                emitter << YAML::Key << "event_type" << YAML::Value << "button";
                                emitter << YAML::Key << "button_state" << YAML::Value << config->_buttons;
                                break;
                            case input_joystick_t::axis:
                                emitter << YAML::Key << "event_type" << YAML::Value << "axis";
                                break;
                            case input_joystick_t::ball:
                                emitter << YAML::Key << "event_type" << YAML::Value << "ball";
                                break;
                        }
                        break;
                    }
                    case input_binding::mouse: {
                        emitter << YAML::Key << "type" << YAML::Value << "mouse";
                        break;
                    }
                    default:
                        break;
                }
                emitter << YAML::EndMap;
            }
            emitter << YAML::EndSeq;
            emitter << YAML::EndMap;
        }
        return !result.is_failed();
    }

    input_action_ptr_list input_action::filtered_catalog(action_type_flags types) {
        input_action_ptr_list list {};

        action_type_flags type = input_action::type::keyboard;
        while (type != input_action::type::last) {
            if ((types & type) == 0) {
                type <<= 1;
                continue;
            }
            auto it = s_catalog_index.find(type);
            if (it != s_catalog_index.end()) {
                auto& actions = it->second;
                for (auto action_ptr : actions)
                    list.push_back(action_ptr);
            }
            type <<= 1;
        }

        return list;
    }

    input_action::input_action(
            action_id id,
            const std::string& name,
            const std::string& category,
            const std::string& description,
            action_type_flags type_value,
            action_flags flag_value) : _id(id),
                                       _name(name),
                                       _category(category),
                                       _description(description),
                                       _flags(flag_value),
                                       _types(type_value) {
    }

    bool input_action::process(
            const SDL_Event* event,
            event_data_t& data) const {
        if (_bindings.empty())
            return false;

        for (const auto& binding : _bindings) {
            if (binding.matches(event, data))
                return true;
        }

        return false;
    }

    action_id input_action::id() const {
        return _id;
    }

    bool input_action::has_bindings() const {
        return !_bindings.empty();
    }
    
    void input_action::bind_quit() {
        _bindings.push_back(input_binding::for_quit());
    }

    void input_action::bind_move() {
        _bindings.push_back(input_binding::for_move());
    }

    void input_action::bind_resize() {
        _bindings.push_back(input_binding::for_resize());
    }

    void input_action::bind_restore() {
        _bindings.push_back(input_binding::for_restore());
    }

    void input_action::bind_minimized() {
        _bindings.push_back(input_binding::for_minimize());
    }

    void input_action::bind_maximized() {
        _bindings.push_back(input_binding::for_maximize());
    }

    void input_action::bind_text_input() {
        _bindings.push_back(input_binding::for_text_input());
    }

    std::string input_action::name() const {
        return _name;
    }

    std::string input_action::category() const {
        return _category;
    }

    std::string input_action::description() const {
        return _description;
    }

    void input_action::bind_keys(input_keys keys) {
        _bindings.push_back(input_binding::for_key_combination(keys));
    }

    action_type_flags input_action::types() const {
        return _types;
    }

    input_action* input_action::find_by_id(action_id id) {
        auto it = std::find_if(
            s_catalog.begin(),
            s_catalog.end(),
            [&id](const input_action& action) {
                return action._id == id;
            });
        return it != s_catalog.end() ? &(*it) : nullptr;
    }

    void input_action::types(input_action::flag::values flags) {
        _types = flags;
    }

    input_action* input_action::find_by_name(const std::string& name) {
        auto it = std::find_if(
            s_catalog.begin(),
            s_catalog.end(),
            [&name](const input_action& action) {
                return action._name == name;
            });
        return it != s_catalog.end() ? &(*it) : nullptr;
    }

    void input_action::bind_joystick_buttons(int32_t id, button_state buttons) {
        _bindings.push_back(input_binding::for_joystick_buttons(id, buttons));
    }

    void input_action::bind_joystick_hat(int32_t id, uint8_t hat_id, hat_state state) {
        _bindings.push_back(input_binding::for_joystick_hat(id, hat_id, state));
    }

}