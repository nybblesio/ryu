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
#include "input_action.h"

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
            pending_event_list& pending_events,
            core::renderer& renderer) {
        renderer.push_clip_rect(_bounds);
        renderer.push_blend_mode(SDL_BLENDMODE_NONE);

        auto& fill_color = _palette[_bg_color];
        renderer.set_color(fill_color);
        renderer.fill_rect(_bounds);

        _stack.draw(dt, renderer);
        _stack.update();

        //_action_provider.process(pending_events);

        renderer.pop_blend_mode();
        renderer.pop_clip_rect();
    }

    void context::add_state(
            core::state* state,
            const state_transition_callable& callback) {
        if (state == nullptr)
            return;
        state->context(this);
        _stack.add_state(state, callback);
        state->initialize(bounds());
    }

    void context::push_state(
            int id,
            const core::parameter_dict& params) {
        _stack.push(id, params);
    }

    bool context::initialize(
            core::result& result,
            const core::rect& bounds) {
        _bounds = bounds;
        return on_initialize(result);
    }

    void context::blackboard(
            const std::string& name,
            const std::string& value) {
        _blackboard[name] = value;
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

    void context::draw(core::renderer& renderer) {
        on_draw(renderer);
    }

    void context::remove_state(core::state* state) {
        if (state == nullptr)
            return;
        state->context(nullptr);
        _stack.remove_state(state);
    }

    void context::on_draw(core::renderer& surface) {
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

}