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

    using hat_state = uint8_t;
    using button_state = uint32_t;

    static constexpr uint8_t hat_centered = SDL_HAT_CENTERED;
    static constexpr uint8_t hat_left = SDL_HAT_LEFT;
    static constexpr uint8_t hat_right = SDL_HAT_RIGHT;
    static constexpr uint8_t hat_up = SDL_HAT_UP;
    static constexpr uint8_t hat_down = SDL_HAT_DOWN;
    static constexpr uint8_t hat_left_up = SDL_HAT_LEFTUP;
    static constexpr uint8_t hat_left_down = SDL_HAT_LEFTDOWN;
    static constexpr uint8_t hat_right_up = SDL_HAT_RIGHTUP;
    static constexpr uint8_t hat_right_down = SDL_HAT_RIGHTDOWN;

    static constexpr uint32_t button_0  = 0b00000000000000000000000000000001;
    static constexpr uint32_t button_1  = 0b00000000000000000000000000000010;
    static constexpr uint32_t button_2  = 0b00000000000000000000000000000100;
    static constexpr uint32_t button_3  = 0b00000000000000000000000000001000;
    static constexpr uint32_t button_4  = 0b00000000000000000000000000010000;
    static constexpr uint32_t button_5  = 0b00000000000000000000000000100000;
    static constexpr uint32_t button_6  = 0b00000000000000000000000001000000;
    static constexpr uint32_t button_7  = 0b00000000000000000000000010000000;
    static constexpr uint32_t button_8  = 0b00000000000000000000000100000000;
    static constexpr uint32_t button_9  = 0b00000000000000000000001000000000;
    static constexpr uint32_t button_10 = 0b00000000000000000000010000000000;
    static constexpr uint32_t button_11 = 0b00000000000000000000100000000000;
    static constexpr uint32_t button_12 = 0b00000000000000000001000000000000;
    static constexpr uint32_t button_13 = 0b00000000000000000010000000000000;
    static constexpr uint32_t button_14 = 0b00000000000000000100000000000000;
    static constexpr uint32_t button_15 = 0b00000000000000001000000000000000;
    static constexpr uint32_t button_16 = 0b00000000000000010000000000000000;
    static constexpr uint32_t button_17 = 0b00000000000000100000000000000000;
    static constexpr uint32_t button_18 = 0b00000000000001000000000000000000;
    static constexpr uint32_t button_19 = 0b00000000000010000000000000000000;
    static constexpr uint32_t button_20 = 0b00000000000100000000000000000000;
    static constexpr uint32_t button_21 = 0b00000000001000000000000000000000;
    static constexpr uint32_t button_22 = 0b00000000010000000000000000000000;
    static constexpr uint32_t button_23 = 0b00000000100000000000000000000000;
    static constexpr uint32_t button_24 = 0b00000001000000000000000000000000;
    static constexpr uint32_t button_25 = 0b00000010000000000000000000000000;
    static constexpr uint32_t button_26 = 0b00000100000000000000000000000000;
    static constexpr uint32_t button_27 = 0b00001000000000000000000000000000;
    static constexpr uint32_t button_28 = 0b00010000000000000000000000000000;
    static constexpr uint32_t button_29 = 0b00100000000000000000000000000000;
    static constexpr uint32_t button_30 = 0b01000000000000000000000000000000;
    static constexpr uint32_t button_31 = 0b10000000000000000000000000000000;

    class joystick {
    public:
        explicit joystick(uint32_t index);

        virtual ~joystick();

        void clear();

        void initialize();

        int32_t id() const;

        uint32_t index() const;

        button_state buttons() const;

        bool button(uint8_t id) const;

        hat_state hat(uint8_t id) const;

        void hat(uint8_t id, hat_state state);

        void button(uint8_t id, bool pressed);

    private:
        int32_t _id;
        uint32_t _index;
        std::string _name;
        uint32_t _number_hats;
        uint32_t _number_axes;
        uint32_t _number_balls;
        uint32_t _number_buttons;
        button_state _buttons = 0;
        SDL_Joystick* _handle = nullptr;
        std::vector<hat_state> _hats {};
    };

    using joystick_list = std::vector<joystick>;
};

