//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#pragma once

#include "core_types.h"

namespace ryu::core {

    class timer_pool {
    public:
        static timer_pool* instance();

        void update();

        void remove_timer(timer* timer);

        void add_timer(core::timer* timer);

    protected:
        timer_pool() = default;

    private:
        timer_list _timers {};
    };

};

