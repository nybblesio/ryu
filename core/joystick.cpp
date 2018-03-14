//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "joystick.h"

namespace ryu::core {

    joystick::joystick(uint32_t index) : _index(index) {
        _handle = SDL_JoystickOpen(index);
    }

    joystick::~joystick() {
        if (SDL_JoystickGetAttached(_handle)) {
            SDL_JoystickClose(_handle);
            _handle = nullptr;
        }
    }

    void joystick::clear() {
        _buttons = 0;
        for (auto i = 0; i < _number_hats; i++)
            _hats[i] = 0;
    }

    void joystick::initialize() {
        _name = SDL_JoystickName(_handle);
        _id = SDL_JoystickInstanceID(_handle);
        _number_axes = static_cast<uint32_t>(SDL_JoystickNumAxes(_handle));
        _number_hats = static_cast<uint32_t>(SDL_JoystickNumHats(_handle));
        _hats.resize(_number_hats);
        _number_balls = static_cast<uint32_t>(SDL_JoystickNumBalls(_handle));
        _number_buttons = static_cast<uint32_t>(SDL_JoystickNumButtons(_handle));
    }

    int32_t joystick::id() const {
        return _id;
    }

    uint32_t joystick::index() const {
        return _index;
    }

    uint32_t joystick::buttons() const {
        return _buttons;
    }

    bool joystick::button(uint8_t id) const {
        return (_buttons & (2^id)) != 0;
    }

    hat_state joystick::hat(uint8_t id) const {
        return _hats[id];
    }

    void joystick::hat(uint8_t id, hat_state state) {
        _hats[id] = state;
    }

    void joystick::button(uint8_t id, bool pressed) {
        if (pressed)
            _buttons |= 2^id;
        else
            _buttons |= ~(2^id);
    }

}