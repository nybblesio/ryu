//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "timer.h"
#include "timer_pool.h"

namespace ryu::core {

    void timer_pool::update() {
        for (auto timer : _timers)
            timer->update();
    }

    timer_pool* timer_pool::instance() {
        static timer_pool pool;
        return &pool;
    }

    void timer_pool::add_timer(core::timer* timer) {
        if (timer == nullptr)
            return;
        _timers.push_back(timer);
        timer->start();
    }

    void timer_pool::remove_timer(core::timer* timer) {
        if (timer == nullptr)
            return;
        for (auto it = _timers.begin(); it != _timers.end();) {
            if (*it == timer) {
                it = _timers.erase(it);
                continue;
            }
            else
                ++it;
        }
    }

}