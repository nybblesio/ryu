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

        void kill();

        void start();

        void reset();

        void update();

        bool dead() const {
            return _killed;
        }

        bool ready() const {
            return _callback != nullptr;
        }

        uint32_t id() const {
            return _id;
        }

        bool is_expired() const {
            return _expired;
        }

        void bind(const timer_callable& callback);

        bool operator< (const timer& rhs) const {
            return _id < rhs._id;
        }

        bool operator== (const timer& rhs) const {
            return _id == rhs._id;
        }

    private:
        uint32_t _id = 0;
        bool _killed = false;
        bool _expired = false;
        uint32_t _duration = 0;
        uint32_t _start_time = 0;
        timer_callable _callback {};
    };

};
