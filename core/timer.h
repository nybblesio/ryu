//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <cstdint>
#include <functional>
#include "core_types.h"

namespace ryu::core {

    class timer {
    public:
        explicit timer(uint32_t duration);

        virtual ~timer();

        int id() const {
            return _id;
        }

        void kill();

        void start();

        void reset();

        void update();

        bool dead() const {
            return _killed;
        }

        bool is_expired() const {
            return _expired;
        }

        void bind(timer_callable callback);

        bool operator< (const timer& rhs) const {
            return _id < rhs._id;
        }

        bool operator== (const timer& rhs) const {
            return _id == rhs._id;
        }

    private:
        int _id = 0;
        bool _killed = false;
        bool _expired = false;
        uint32_t _duration = 0;
        uint32_t _start_time = 0;
        timer_callable _callback;
    };

};
