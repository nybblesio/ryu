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

#include <vector>
#include <SDL.h>
#include "core_types.h"

namespace ryu::core {

    class state_stack {
    public:
        struct pending_state {
            enum action {
                none = 1,
                push,
                pop
            };
        };

        void clear();

        void update();

        int peek() const;

        bool empty() const;

        void draw(uint32_t dt);

        void pop(int to_id = -1);

        inline core::state* active() {
            return _active;
        }

        core::state* find_state(int id);

        void add_state(core::state* state);

        void remove_state(core::state* state);

        void push(int id, const core::parameter_dict& params);

        void add_state(core::state* state, const state_transition_callable& callback);

    private:
        void update_active_state();

    private:
        state_dict _states;
        int _pending_id = -1;
        std::vector<int> _stack;
        core::state* _active = nullptr;
        core::parameter_dict _pending_params {};
        pending_state::action _pending_action = pending_state::action::none;
    };

};

