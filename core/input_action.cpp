//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <vector>
#include <logger.h>
#include "id_pool.h"
#include "input_action.h"

namespace ryu::core {

    input_action_catalog input_action::s_catalog {};
    static log4cpp::Category* s_log = logger::instance()->category(
        "input_action",
        log4cpp::Priority::INFO);

    void input_action::initialize() {
        s_catalog.reserve(1024);
    }

    input_action* input_action::create(
            const std::string& name,
            const std::string& category,
            const std::string& description) {
        auto action = find_by_name(name);
        if (action != nullptr)
            return action;
        s_catalog.emplace_back(
            id_pool::instance()->allocate(),
            name,
            category,
            description);
        return &s_catalog.back();
    }

    input_action* input_action::create_no_map(
            const std::string& name,
            const std::string& category,
            const std::string& description) {
        s_catalog.emplace_back(
            id_pool::instance()->allocate(),
            name,
            category,
            description,
            flags::no_map);
        return &s_catalog.back();
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
            auto flags = static_cast<uint8_t>(action_node["flags"].as<uint32_t>());
            auto name = action_node["name"].as<std::string>();
            auto category = action_node["category"].as<std::string>();
            auto description = action_node["description"].as<std::string>();

            id_pool::instance()->mark_used(action_id);
            s_catalog.emplace_back(
                action_id,
                name,
                category,
                description);
            auto action = &s_catalog.back();
            action->_flags = flags;

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

    input_action::input_action(
            action_id id,
            const std::string& name,
            const std::string& category,
            const std::string& description,
            action_flags flag_value) : _id(id),
                                       _name(name),
                                       _category(category),
                                       _description(description),
                                       _flags(flag_value) {
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

    void input_action::register_handler(
            action_sink_type type,
            const input_action_filter& filter,
            const input_action_perform& perform) {
        _handlers.insert(std::make_pair(
            type,
            input_action_handler_t {filter, perform}));
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

    action_id input_action::type() const {
        return _id;
    }

    std::string input_action::name() const {
        return _name;
    }

    std::string input_action::category() const {
        return _category;
    }

    action_sink_type input_action::process_action(
            const input_binding& binding,
            const event_data_t& data) const {
        for (uint16_t i = action_sink::last; i > action_sink::none; i--) {
            auto it = _handlers.find(i);
            if (it == _handlers.end())
                continue;
//            s_log->info(fmt::format("name: {}, action_sink: {}", name(), i));
            if (!it->second.filter(data))
                continue;
//            s_log->info("...perform");
            if (it->second.perform(data))
                return (action_sink_type) i;
        }
        return action_sink::none;
    }

    std::string input_action::description() const {
        return _description;
    }

    void input_action::bind_keys(input_keys keys) {
        _bindings.push_back(input_binding::for_key_combination(keys));
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

    input_action* input_action::find_by_name(const std::string& name) {
        auto it = std::find_if(
            s_catalog.begin(),
            s_catalog.end(),
            [&name](const input_action& action) {
                return action._name == name;
            });
        return it != s_catalog.end() ? &(*it) : nullptr;
    }

    action_sink_type input_action::process(const SDL_Event* event) const {
        if (_bindings.empty())
            return action_sink::none;

        event_data_t data {};

        for (const auto& binding : _bindings) {
            if (binding.matches(event, data))
                return process_action(binding, data);
        }

        return action_sink::none;
    }

    void input_action::bind_joystick_buttons(int32_t id, button_state buttons) {
        _bindings.push_back(input_binding::for_joystick_buttons(id, buttons));
    }

    void input_action::bind_joystick_hat(int32_t id, uint8_t hat_id, hat_state state) {
        _bindings.push_back(input_binding::for_joystick_hat(id, hat_id, state));
    }

}