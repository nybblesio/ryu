//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include <SDL_timer.h>
#include "timer.h"
#include "id_pool.h"

namespace ryu::core {

    timer::timer(uint32_t duration) : _duration(duration) {
        _id = core::id_pool::instance()->allocate();
    }

    timer::~timer() {
        core::id_pool::instance()->release(_id);
    }

    void timer::kill() {
        _killed = true;
    }

    void timer::start() {
        _start_time = SDL_GetTicks();
        _expired = false;
    }

    void timer::reset() {
        start();
    }

    void timer::update() {
        if (_expired)
            return;
        if (SDL_GetTicks() - _start_time > _duration) {
            _expired = true;
            if (_callback != nullptr)
                _callback(this);
        }
    }

    void timer::bind(const timer_callable& callback) {
        _callback = callback;
    }

}