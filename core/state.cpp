//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include "state.h"
#include "engine.h"
#include "context.h"

namespace ryu::core {

    state::state(
            core::context* context,
            int id,
            const std::string& name,
            bool render_parent) : _id(id),
                                  _name(name),
                                  _render_parent(render_parent),
                                  _context(context) {
    }

    void state::end_state() {
        _context->pop_state();
    }

    void state::update(uint32_t dt) {
        on_update(dt);
    }

    void state::init(SDL_Renderer* renderer) {
        if (!_initialized) {
            on_init(renderer);
            _initialized = true;
        }
    }

    void state::draw(SDL_Renderer* renderer) {
        on_draw(renderer);
    }

    bool state::process_event(const SDL_Event* e) {
        return on_process_event(e);
    }

    void state::erase_blackboard(const std::string& name) {
        _context->erase_blackboard(name);
    }

    std::string state::blackboard(const std::string& name) const {
        return _context->blackboard(name);
    }

    void state::blackboard(const std::string& name, const std::string& value) {
        _context->blackboard(name, value);
    }

}