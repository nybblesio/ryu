//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <SDL_joystick.h>
#include "joysticks.h"

namespace ryu::core {

    joysticks* joysticks::instance() {
        static joysticks s_instance;
        return &s_instance;
    }

    void joysticks::shutdown() {
        for (const auto& device : _devices) {
            SDL_JoystickClose(device.joystick);
        }
        _devices.clear();
    }

    void joysticks::initialize() {
        auto number_of_joysticks = SDL_NumJoysticks();

        for (auto i = 0; i < number_of_joysticks; i++) {
            auto joystick = SDL_JoystickOpen(i);
            if (joystick != nullptr) {
                _devices.emplace_back(
                    SDL_JoystickInstanceID(joystick),
                    SDL_JoystickName(joystick),
                    SDL_JoystickNumHats(joystick),
                    SDL_JoystickNumAxes(joystick),
                    SDL_JoystickNumBalls(joystick),
                    SDL_JoystickNumButtons(joystick),
                    joystick);
            }
        }
    }

    const joystick_t* joysticks::device(uint16_t index) const {
        if (index < _devices.size())
            return &_devices[index];
        return nullptr;
    }

}