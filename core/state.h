//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <map>
#include <string>
#include <SDL_events.h>
#include <SDL_render.h>
#include "view.h"
#include "renderer.h"
#include "core_types.h"

namespace ryu::core {

    class state : public view_host {
    public:
        explicit state(const std::string& name, bool render_parent = false);

        ~state() override;

        void update(
                uint32_t dt,
                pending_event_list& events);

        void initialize(
                core::result& result,
                const core::rect& bounds);

        void deactivate();

        bool transition_to(
            core::state_transition_command command,
            const parameter_dict& params);

        uint32_t id() const;

        core::context* context();

        std::string name() const;

        bool render_parent() const;

        bool is_initialized() const;

        bool load(core::result& result);

        bool is_focused() const override;

        bool is_visible() const override;

        void context(core::context* value);

        void draw(core::renderer& renderer);

        void resize(const core::rect& bounds);

        inline core::context* context() const {
            return _context;
        }

        void remove_change_listener(uint32_t id);

        bool operator< (const state& rhs) const {
            return _id < rhs._id;
        }

        bool operator== (const state& rhs) const {
            return _id == rhs._id;
        }

        inline core::preferences* prefs() override {
            return _context->prefs();
        }

        void activate(const core::parameter_dict& params);

        uint32_t add_change_listener(const state_change_callable& callable);

        void transition_callback(const state_transition_callable& callback);

    protected:
        void end_state();

        virtual void on_update(
                uint32_t dt,
                pending_event_list& events) = 0;

        virtual void on_deactivate();

        virtual void on_initialize() = 0;

        virtual bool on_load(core::result& result) = 0;

        void erase_blackboard(const std::string& name);

        core::input_action_provider& action_provider();

        virtual void on_resize(const core::rect& bounds);

        virtual void on_draw(core::renderer& renderer) = 0;

        void raise_change_event(change_reason_flags reasons);

        std::string blackboard(const std::string& name) const;

        virtual void on_activate(const core::parameter_dict& params);

        void blackboard(const std::string& name, const std::string& value);

    private:
        uint32_t _id = 0;
        std::string _name;
        bool _initialized = false;
        bool _render_parent = false;
        core::context* _context = nullptr;
        state_transition_callable _callback {};
        core::input_action_provider _action_provider;
        std::map<uint32_t, state_change_callable> _listeners {};
    };
};
