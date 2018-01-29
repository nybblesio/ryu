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
#include "engine.h"
#include "context.h"
#include "id_pool.h"
#include "environment.h"

namespace ryu::core {

    context::context(const std::string& name) : _id(core::id_pool::instance()->allocate()),
                                                _name(name),
                                                _environment(std::make_unique<core::environment>()) {
    }

    context::~context() {
        core::id_pool::instance()->release(_id);
    }

    void context::update(
            uint32_t dt,
            core::renderer& renderer,
            std::deque<SDL_Event>& events) {
        renderer.push_clip_rect(_bounds);
        renderer.push_blend_mode(SDL_BLENDMODE_NONE);

        auto current_state_id = _stack.peek();
        if (current_state_id != -1) {
            auto active = _stack.active();
            if (_engine->focus() == _id) {
                while (!events.empty()) {
                    auto processed = active->process_event(&events.front());
                    if (!processed) {
                        // XXX: We may need to do something with this event
                    }
                    events.pop_front();
                }
            }
        }

        auto pal = *palette();
        auto& fill_color = pal[_bg_color];
        renderer.set_color(fill_color);
        renderer.fill_rect(_bounds);

        _stack.draw(dt, renderer);
        _stack.update();

        renderer.pop_blend_mode();
        renderer.pop_clip_rect();
    }

    void context::resize() {
        auto active = _stack.find_state(_stack.peek());
        if (active != nullptr)
            active->resize(bounds());
    }

    uint32_t context::id() const {
        return _id;
    }

    void context::pop_state(int to_id) {
        _stack.pop(to_id);
    }

    const core::font_t* context::font_face() {
        return _font;
    }

    core::font_family* context::font_family() {
        return _family;
    }

    void context::add_state(core::state* state) {
        if (state == nullptr)
            return;
        state->context(this);
        _stack.add_state(state);
        state->initialize(bounds());
    }

    void context::palette(core::palette* palette) {
        _palette = palette;
    }

    void context::remove_state(core::state* state) {
        if (state == nullptr)
            return;
        state->context(nullptr);
        _stack.remove_state(state);
    }

    bool context::on_initialize(core::result& result) {
        return false;
    }

    void context::font_face(const core::font_t* value) {
        _font = value;
    }

    void context::font_family(core::font_family* value) {
        _family = value;
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

    void context::push_state(int id, const core::parameter_dict& params) {
        _stack.push(id, params);
    }

    bool context::initialize(core::result& result, const core::rect& bounds) {
        _bounds = bounds;
        return on_initialize(result);
    }

    void context::blackboard(const std::string& name, const std::string& value) {
        _blackboard[name] = value;
    }

    void context::add_state(core::state* state, const state_transition_callable& callback) {
        if (state == nullptr)
            return;
        state->context(this);
        _stack.add_state(state, callback);
        state->initialize(bounds());
    }

}