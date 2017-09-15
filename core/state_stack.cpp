//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "state.h"
#include "state_stack.h"

namespace ryu::core {

    void state_stack::clear() {
        _stack.clear();
        _pending_id = -1;
        _active = nullptr;
        _pending_action = pending_state::action::none;
    }

    void state_stack::update() {
        if (_pending_id == -1)
            return;

        switch (_pending_action) {
            case pending_state::action::push: {
                _stack.push_back(_pending_id);
                update_active_state();
                break;
            }
            case pending_state::action::pop: {
                while (!_stack.empty()) {
                    auto top = _stack.back();
                    _stack.pop_back();
                    if (top == _pending_id)
                        break;
                }
                update_active_state();
                break;
            }
            default: {
                break;
            }
        }

        _pending_action = pending_state::action::none;
        _pending_id = -1;
    }

    int state_stack::peek() const {
        return _stack.empty() ? -1 : _stack.back();
    }

    void state_stack::push(int id) {
        _pending_action = pending_state::action::push;
        _pending_id = id;
    }

    bool state_stack::empty() const {
        return _stack.empty();
    }

    void state_stack::pop(int to_id) {
        _pending_action = pending_state::action::pop;
        _pending_id = to_id != -1 ? to_id : peek();
    }

    void state_stack::draw(uint32_t dt) {
        if (_stack.empty())
            return;

        auto count = 1;
        for (auto it = _stack.rbegin(); it != _stack.rend(); ++it) {
            auto state = find_state(*it);
            if (!state->render_parent())
                break;
            count++;
        }

        for (auto i = _stack.size() - count; i < _stack.size(); i++) {
            auto state = find_state(_stack[i]);
            state->update(dt);
            state->draw();
        }
    }

    void state_stack::update_active_state() {
        _active = find_state(peek());
    }

    core::state* state_stack::find_state(int id) {
        core::state* state = nullptr;

        auto it = _states.find(id);
        if (it != _states.end())
            state = it->second.instance;

        return state;
    }

    void state_stack::add_state(core::state* state) {
        if (state == nullptr)
            return;
        _states.insert(std::make_pair(state->id(), state_t {state}));
    }

    void state_stack::remove_state(core::state* state) {
        if (state == nullptr)
            return;
        _states.erase(state->id());
    }

    void state_stack::add_state(core::state* state, const state_transition_callable& callback) {
        if (state == nullptr)
            return;
        state->transition_callback(callback);
        _states.insert(std::make_pair(state->id(), state_t {state}));
    }

}