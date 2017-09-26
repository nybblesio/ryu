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
#include "core_types.h"

namespace ryu::core {

    class state {
    public:
        state(
                core::context* context,
                int id,
                const std::string& name,
                bool render_parent = false);

        virtual ~state() = default;

        void draw();

        int id() const;

        void resize();

        void initialize();

        void deactivate();

        void update(uint32_t dt);

        std::string name() const;

        bool render_parent() const;

        bool is_initialized() const;

        bool process_event(const SDL_Event* e);

        inline core::context* context() const {
            return _context;
        }

        bool operator< (const state& rhs) const {
            return _id < rhs._id;
        }

        bool operator== (const state& rhs) const {
            return _id == rhs._id;
        }

        void activate(const core::parameter_dict& params);

        void transition_callback(const state_transition_callable& callback);

        bool transition_to(const std::string& name, const parameter_dict& params);

    protected:
        void end_state();

        virtual void on_resize();

        virtual void on_draw() = 0;

        virtual void on_deactivate();

        virtual void on_initialize() = 0;

        virtual void on_update(uint32_t dt) = 0;

        void erase_blackboard(const std::string& name);

        virtual bool on_process_event(const SDL_Event* e) = 0;

        std::string blackboard(const std::string& name) const;

        virtual void on_activate(const core::parameter_dict& params);

        void blackboard(const std::string& name, const std::string& value);

    private:
        int _id;
        std::string _name;
        bool _initialized = false;
        bool _render_parent = false;
        state_transition_callable _callback;
        ryu::core::context* _context = nullptr;
    };
};
