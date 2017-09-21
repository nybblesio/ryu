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

namespace ryu::core {

    class context {
    public:
        context(int id, std::string name);

        virtual ~context() = default;

        int id() const;

        void initialize(
                core::engine* engine,
                const SDL_Rect& clip_rect,
                uint8_t color_index);

        void update(uint32_t dt);

        inline int peek_state() const {
            return _stack.peek();
        }

        void pop_state(int to_id = -1);

        void remove_timer(timer* timer);

        void add_timer(core::timer* timer);

        void add_state(core::state* state);

        inline SDL_Rect clip_rect() const {
            return _clip_rect;
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

        void erase_blackboard(const std::string& name);

        std::string blackboard(const std::string& name) const;

        void push_state(int id, const core::parameter_dict& params);

        void blackboard(const std::string& name, const std::string& value);

        void add_state(core::state* state, const state_transition_callable& callback);

    private:
        int _id;
        std::string _name;
        timer_list _timers;
        SDL_Rect _clip_rect;
        core::state_stack _stack;
        uint8_t _fill_color_index;
        core::blackboard _blackboard;
        core::engine* _engine = nullptr;
        SDL_Renderer* _renderer = nullptr;
        core::palette* _palette = nullptr;
    };

};

