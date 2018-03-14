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
#include "id_pool.h"
#include "input_action.h"

namespace ryu::core {

    input_action_catalog input_action::s_catalog {};

    void input_action::initialize() {
        s_catalog.reserve(1024);
    }

    input_action* input_action::create(
            const std::string& name,
            const std::string& category,
            const std::string& description) {
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

    const input_action_catalog& input_action::catalog() {
        return s_catalog;
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
            if (!it->second.filter(data))
                continue;
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