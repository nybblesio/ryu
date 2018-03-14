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
        _devices.clear();
    }

    void joysticks::initialize() {
        auto number_of_joysticks = SDL_NumJoysticks();
        for (auto i = 0; i < number_of_joysticks; i++) {
            _devices.emplace_back(i);
            _devices.back().initialize();
        }
    }

    joystick* joysticks::device_by_id(int32_t id) {
        auto it = std::find_if(
            _devices.begin(),
            _devices.end(),
            [&id](const joystick& joy) {
                return joy.id() == id;
            });
        return it != _devices.end() ? &(*it) : nullptr;
    }

    joystick* joysticks::device_by_index(uint32_t index) {
        if (index < _devices.size())
            return &_devices[index];
        return nullptr;
    }

}