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

        int id() const {
            return _id;
        }

        void update(uint32_t dt);

        std::string name() const {
            return _name;
        }

        bool render_parent() const {
            return _render_parent;
        }

        bool is_initialized() const {
            return _initialized;
        }

        core::context* context() const {
            return _context;
        }

        void init(SDL_Renderer* renderer);

        void draw(SDL_Renderer* renderer);

        bool process_event(const SDL_Event* e);

        bool operator< (const state& rhs) const {
            return _id < rhs._id;
        }

        bool operator== (const state& rhs) const {
            return _id == rhs._id;
        }

        bool transition_to(const std::string& name) {
            if (_callback)
                return _callback(this, name);
            return false;
        }

        void transition_callback(const state_transition_callable& callback) {
            _callback = callback;
        }

    protected:
        void end_state();

        virtual void on_update(uint32_t dt) = 0;

        void erase_blackboard(const std::string& name);

        virtual void on_init(SDL_Renderer* renderer) = 0;

        virtual void on_draw(SDL_Renderer* renderer) = 0;

        virtual bool on_process_event(const SDL_Event* e) = 0;

        std::string blackboard(const std::string& name) const;

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
