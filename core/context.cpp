//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <SDL.h>
#include <utility>
#include "state.h"
#include "timer.h"
#include "context.h"
#include "engine.h"

namespace ryu::core {

    context::context(int id, std::string name) : _id(id),
                                                 _name(std::move(name)) {
    }

    int context::id() const {
        return _id;
    }

    void context::on_resize() {
    }

    void context::initialize(
            core::engine* engine,
            const core::rect& bounds,
            uint8_t color_index) {
        _engine = engine;
        _bounds = bounds;
        _renderer = engine->_renderer;
        _fill_color_index = color_index;
    }

    void context::pop_state(int to_id) {
        _stack.pop(to_id);
    }

    void context::add_timer(core::timer* timer) {
        if (timer == nullptr)
            return;
        _timers.push_back(timer);
        timer->start();
    }

    void context::add_state(core::state* state) {
        if (state == nullptr)
            return;
        _stack.add_state(state);
    }

    void context::palette(core::palette* palette) {
        _palette = palette;
    }

    void context::remove_state(core::state* state) {
        _stack.remove_state(state);
    }

    void context::remove_timer(core::timer* timer) {
        if (timer == nullptr)
            return;
        timer->kill();
    }

    void context::erase_blackboard(const std::string& name) {
        _blackboard.erase(name);
    }

    std::string context::blackboard(const std::string& name) const {
        auto it = _blackboard.find(name);
        if (it != _blackboard.end()) {
            return it->second;
        }
        return "";
    }

    void context::update(uint32_t dt, std::deque<SDL_Event>& events) {
        auto bounds = _bounds.to_sdl_rect();
        SDL_RenderSetClipRect(_renderer, &bounds);
        SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_NONE);

        for (auto it = _timers.begin(); it != _timers.end();) {
            auto timer = *it;
            timer->update();
            if (timer->dead())
                it = _timers.erase(it);
            else
                ++it;
        }

        auto current_state_id = _stack.peek();
        if (current_state_id != -1) {
            auto active = _stack.active();
            if (!active->is_initialized())
                active->initialize();

            if (_engine->_focused_context == _id) {
                while (!events.empty()) {
                    auto processed = active->process_event(&events.front());
                    if (!processed) {
                        // XXX: We may need to do something with this event
                    }
                    events.pop_front();
                }
            }
        }

        auto& fill_color = (*palette())[_fill_color_index];
        SDL_SetRenderDrawColor(
                _renderer,
                fill_color.red(),
                fill_color.green(),
                fill_color.blue(),
                fill_color.alpha());
        SDL_RenderFillRect(_renderer, &bounds);

        _stack.draw(dt);
        _stack.update();
    }

    void context::push_state(int id, const core::parameter_dict& params) {
        _stack.push(id, params);
    }

    void context::blackboard(const std::string& name, const std::string& value) {
        _blackboard[name] = value;
    }

    void context::add_state(core::state* state, const state_transition_callable& callback) {
        if (state == nullptr)
            return;
        _stack.add_state(state, callback);
    }

}