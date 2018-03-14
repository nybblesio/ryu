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

#include <string>
#include <vector>
#include <SDL_joystick.h>

namespace ryu::core {

    struct joystick_t {
        joystick_t(
            int32_t id,
            const std::string& name,
            int32_t number_hats,
            int32_t number_axes,
            int32_t number_balls,
            int32_t number_buttons,
            SDL_Joystick* joystick) : id(id),
                                      name(name),
                                      number_hats(number_hats),
                                      number_axes(number_axes),
                                      number_balls(number_balls),
                                      number_buttons(number_buttons),
                                      joystick(joystick) {
        }

        int32_t id;
        std::string name;
        int32_t number_hats;
        int32_t number_axes;
        int32_t number_balls;
        int32_t number_buttons;
        SDL_Joystick* joystick;
    };

    using joystick_list = std::vector<joystick_t>;

    class joysticks {
    public:
        static joysticks* instance();

        void shutdown();

        void initialize();

        const joystick_t* device(uint16_t index) const;

    private:
        joystick_list _devices {};
    };

};

