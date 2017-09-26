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

#include <string>
#include "palette.h"
#include "state_stack.h"
#include "rect.h"

namespace ryu::core {

    class context {
    public:
        context(int id, std::string name);

        virtual ~context() = default;

        int id() const;

        void initialize(
                core::engine* engine,
                const core::rect& bounds,
                uint8_t color_index);

        inline int peek_state() const {
            return _stack.peek();
        }

        void pop_state(int to_id = -1);

        void remove_timer(timer* timer);

        void add_timer(core::timer* timer);

        void add_state(core::state* state);

        inline core::rect bounds() const {
            return _bounds;
        }

        void palette(core::palette* palette);

        void remove_state(core::state* state);

        inline core::engine* engine() const {
            return _engine;
        }

        inline SDL_Renderer* renderer() const {
            return _renderer;
        }

        inline core::palette* palette() const {
            return _palette;
        }

        inline core::state* find_state(int id) {
            return _stack.find_state(id);
        }

        inline void bounds(const core::rect& value) {
            _bounds = value;
            on_resize();
        }

        void erase_blackboard(const std::string& name);

        std::string blackboard(const std::string& name) const;

        void update(uint32_t dt, std::deque<SDL_Event>& events);

        void push_state(int id, const core::parameter_dict& params);

        void blackboard(const std::string& name, const std::string& value);

        void add_state(core::state* state, const state_transition_callable& callback);

    protected:
        virtual void on_resize();

    private:
        int _id;
        std::string _name;
        timer_list _timers;
        core::rect _bounds;
        core::state_stack _stack;
        uint8_t _fill_color_index;
        core::blackboard _blackboard;
        core::engine* _engine = nullptr;
        SDL_Renderer* _renderer = nullptr;
        core::palette* _palette = nullptr;
    };

};

