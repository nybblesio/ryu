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

        uint32_t id() const;

        void deactivate();

        core::context* context();

        void update(uint32_t dt);

        std::string name() const;

        bool render_parent() const;

        bool is_initialized() const;

        bool is_focused() const override;

        bool is_visible() const override;

        void context(core::context* value);

        void draw(core::renderer& renderer);

        void resize(const core::rect& bounds);

        inline core::context* context() const {
            return _context;
        }

        bool operator< (const state& rhs) const {
            return _id < rhs._id;
        }

        void initialize(const core::rect& bounds);

        bool operator== (const state& rhs) const {
            return _id == rhs._id;
        }

        void activate(const core::parameter_dict& params);

        void on_change(const state_change_callable& callable);

        void transition_callback(const state_transition_callable& callback);

        bool transition_to(const std::string& name, const parameter_dict& params);

    protected:
        void end_state();

        virtual void on_deactivate();

        virtual void on_initialize() = 0;

        virtual void on_update(uint32_t dt) = 0;

        void erase_blackboard(const std::string& name);

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
        std::vector<state_change_callable> _listeners {};
    };
};
