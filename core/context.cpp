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

    void context::initialize(
            core::engine* engine,
            const SDL_Rect& clip_rect,
            uint8_t color_index) {
        _engine = engine;
        _clip_rect = clip_rect;
        _fill_color_index = color_index;
    }

    core::engine* context::engine() {
        return _engine;
    }

    void context::push_state(int id) {
        _stack.push(id);
    }

    core::palette* context::palette() {
        return _palette;
    }

    void context::update(uint32_t dt) {
        SDL_RenderSetClipRect(_engine->_renderer, &_clip_rect);

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
                active->init(_engine->_renderer);

            if (_engine->_focused_context == _id) {
                SDL_Event e{};
                while (SDL_PollEvent(&e) != 0) {
                    if (e.type == SDL_QUIT) {
                        _engine->_quit = true;
                        break;
                    }

                    auto processed = active->process_event(&e);
                    if (!processed) {
                        // XXX: We may need to do something with this event
                    }
                }
            }
        }

        auto& fill_color = (*palette())[_fill_color_index];
        SDL_SetRenderDrawColor(
                _engine->_renderer,
                fill_color.red(),
                fill_color.green(),
                fill_color.blue(),
                fill_color.alpha());
        SDL_RenderFillRect(_engine->_renderer, &_clip_rect);

        _stack.draw(dt, _engine->_renderer);
        _stack.update();
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

    void context::blackboard(const std::string& name, const std::string& value) {
        _blackboard[name] = value;
    }

    void context::add_state(core::state* state, const state_transition_callable& callback) {
        if (state == nullptr)
            return;
        _stack.add_state(state, callback);
    }

}