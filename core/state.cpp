//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include "state.h"
#include "engine.h"
#include "context.h"
#include "id_pool.h"

namespace ryu::core {

    state::state(
            core::context* context,
            const std::string& name,
            bool render_parent) : _id(core::id_pool::instance()->allocate()),
                                  _name(name),
                                  _render_parent(render_parent),
                                  _context(context) {
    }

    state::~state() {
        core::id_pool::instance()->release(_id);
    }

    void state::draw() {
        on_draw();
    }

    void state::resize() {
        on_resize();
    }

    int state::id() const {
        return _id;
    }

    void state::end_state() {
        _context->pop_state();
    }

    void state::initialize() {
        if (!_initialized) {
            on_initialize();
            _initialized = true;
        }
    }

    void state::on_resize() {
    }

    void state::deactivate() {
        on_deactivate();
    }

    void state::on_deactivate() {
    }

    void state::update(uint32_t dt) {
        on_update(dt);
    }

    std::string state::name() const {
        return _name;
    }

    bool state::render_parent() const {
        return _render_parent;
    }

    bool state::is_initialized() const {
        return _initialized;
    }

    bool state::process_event(const SDL_Event* e) {
        return on_process_event(e);
    }

    void state::erase_blackboard(const std::string& name) {
        _context->erase_blackboard(name);
    }

    void state::activate(const core::parameter_dict& params) {
        on_activate(params);
    }

    void state::on_activate(const core::parameter_dict& params) {
    }

    std::string state::blackboard(const std::string& name) const {
        return _context->blackboard(name);
    }

    void state::blackboard(const std::string& name, const std::string& value) {
        _context->blackboard(name, value);
    }

    void state::transition_callback(const state_transition_callable& callback) {
        _callback = callback;
    }

    bool state::transition_to(const std::string& name, const parameter_dict& params) {
        if (_callback)
            return _callback(this, name, params);
        return false;
    }

}