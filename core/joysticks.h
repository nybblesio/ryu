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

#include <vector>
#include <string>
#include <utility>
#include <SDL2/SDL_joystick.h>
#include "joystick.h"

namespace ryu::core {

    class joysticks {
    public:
        static joysticks* instance();

        void shutdown();

        void initialize();

        joystick* device_by_id(int32_t index);

        joystick* device_by_index(uint32_t index);

    private:
        joystick_list _devices {};
    };

};

