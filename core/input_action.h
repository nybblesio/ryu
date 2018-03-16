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
#include <cstdint>
#include <functional>
#include <SDL_events.h>
#include <yaml-cpp/yaml.h>
#include "result.h"
#include "input_binding.h"

namespace ryu::core {

    using action_id = uint32_t;
    using action_flags = uint8_t;
    using action_sink_type = uint16_t;

    class input_action;

    using input_action_catalog = std::vector<input_action>;
    using input_action_filter = std::function<bool (const event_data_t&)>;
    using input_action_perform = std::function<bool (const event_data_t&)>;

    struct input_action_handler_t {
        input_action_filter filter;
        input_action_perform perform;
    };

    struct action_sink {
        enum types {
            none = 1,
            engine,
            context,
            controller,
            view,

            // this item must always be last
            last
        };

        static bool default_filter(const event_data_t& data) {
            return true;
        }

        static bool default_perform(const event_data_t& data) {
            return true;
        }
    };

    using input_action_handlers = std::map<action_sink_type, input_action_handler_t>;

    class input_action {
    public:
        enum flags {
            none   = 0b00000000,
            no_map = 0b00000001,
        };

        static void initialize();

        static input_action* create(
            const std::string& name,
            const std::string& category,
            const std::string& description);

        static input_action* create_no_map(
            const std::string& name,
            const std::string& category,
            const std::string& description);

        static bool exists(const std::string& name);

        static const input_action_catalog& catalog();

        static input_action* find_by_id(action_id id);

        static bool load(core::result& result, YAML::Node& root);

        static input_action* find_by_name(const std::string& name);

        static bool save(core::result& result, YAML::Emitter& emitter);

        input_action(
            action_id id,
            const std::string& name,
            const std::string& category,
            const std::string& description,
            action_flags flag_value = flags::none);

        void bind_quit();

        void bind_move();

        void bind_resize();

        void bind_restore();

        void bind_minimized();

        void bind_maximized();

        void bind_text_input();

        void register_handler(
            action_sink_type type,
            const input_action_filter& filter,
            const input_action_perform& perform);

        action_id type() const;

        std::string name() const;

        bool has_bindings() const;

        std::string category() const;

        inline bool can_map() const {
            return (_flags & flags::no_map) == 0;
        }

        std::string description() const;

        void bind_keys(input_keys keys);

        action_sink_type process(const SDL_Event* event) const;

        void bind_joystick_buttons(int32_t id, button_state buttons);

        void bind_joystick_hat(int32_t id, uint8_t hat_id, hat_state state);

    private:
        action_sink_type process_action(
            const input_binding& binding,
            const event_data_t& data) const;

    private:
        static input_action_catalog s_catalog;

        action_id _id;
        std::string _name;
        std::string _category;
        std::string _description;
        input_bindings _bindings {};
        action_flags _flags = flags::none;
        input_action_handlers _handlers {};
    };

};