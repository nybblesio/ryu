//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#pragma once

#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <SDL_events.h>
#include <yaml-cpp/yaml.h>
#include "result.h"
#include "input_binding.h"

namespace ryu::core {

    class input_action;

    using action_id = uint32_t;
    using action_flags = uint8_t;
    using action_type_flags = uint16_t;
    using input_action_catalog = std::vector<input_action>;
    using input_action_ptr_list = std::vector<input_action*>;
    using input_action_index = std::map<action_type_flags, input_action_ptr_list>;

    class input_action {
    public:
        struct flag {
            enum values : uint8_t {
                none   = 0b00000000,
                no_map = 0b00000001,
            };
        };

        struct type { ;
            enum values : uint16_t {
                none     = 0b00000000,
                keyboard = 0b00000001,
                joystick = 0b00000010,
                mouse    = 0b00000100,
                window   = 0b00001000,
                system   = 0b00010000,

                last     = 0b10000000
            };
        };

        static input_action* create(
            const std::string& name,
            const std::string& category,
            const std::string& description,
            action_type_flags types = input_action::type::keyboard,
            action_flags flags = input_action::flag::none);

        static input_action* create_no_map(
            const std::string& name,
            const std::string& category,
            const std::string& description,
            action_type_flags types = input_action::type::keyboard);

        static bool exists(const std::string& name);

        static const input_action_catalog& catalog();

        static input_action* find_by_id(action_id id);

        static bool load(core::result& result, YAML::Node& root);

        static input_action* find_by_name(const std::string& name);

        static bool save(core::result& result, YAML::Emitter& emitter);

        static input_action_ptr_list filtered_catalog(action_type_flags types);

        input_action(
            action_id id,
            const std::string& name,
            const std::string& category,
            const std::string& description,
            action_type_flags type_value = type::keyboard,
            action_flags flag_value = flag::none);

        bool process(
            const SDL_Event* event,
            event_data_t& data) const;

        void bind_quit();

        void bind_move();

        void bind_resize();

        void bind_restore();

        action_id id() const;

        void bind_minimized();

        void bind_maximized();

        void bind_text_input();

        std::string name() const;

        bool has_bindings() const;

        std::string category() const;

        inline bool can_map() const {
            return (_flags & flag::no_map) == 0;
        }

        std::string description() const;

        action_type_flags types() const;

        void bind_keys(input_keys keys);

        void types(input_action::flag::values flags);

        void bind_joystick_buttons(int32_t id, button_state buttons);

        void bind_joystick_hat(int32_t id, uint8_t hat_id, hat_state state);

    private:
        static input_action* inner_create(
                uint32_t id,
                const std::string& name,
                const std::string& category,
                const std::string& description,
                action_type_flags types = input_action::type::keyboard,
                action_flags flags = input_action::flag::none);

        static input_action_catalog s_catalog;

        static input_action_index s_catalog_index;

        action_id _id;
        std::string _name;
        std::string _category;
        std::string _description;
        input_bindings _bindings {};
        action_flags _flags = flag::none;
        action_type_flags _types = type::keyboard;
    };

};