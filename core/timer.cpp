//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include <SDL_timer.h>
#include "timer.h"

namespace ryu::core {

    timer::timer(int id, uint32_t duration) : _id(id),
                                              _duration(duration) {
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

    void timer::bind(timer_callable callback) {
        _callback = std::move(callback);
    }

}