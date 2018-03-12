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
#include <SDL_events.h>

namespace ryu::core {

    // emulator / game play
    //
    //  - assembly based games, looking at memory map for input
    //      -> map keyboard, mouse, joystick to input integrated_circuit
    //

    // ide
    //
    // mouse input
    // keyboard combinations
    //
    // raw text input
    //
    // maybe these should be hard coded?
    // ----------------------------------
    //
    // UI field navigation/activation, e.g. tab/shift-tab to move between fields
    // enter/space to activate/selection
    //

    class input_binding;

    using input_bindings = std::vector<input_binding>;

    using input_key = uint32_t;
    using input_keys = std::vector<input_key>;

    using joystick_button = uint8_t;
    using joystick_buttons = std::vector<joystick_button>;

    struct input_joystick_t {
        int32_t id;
        joystick_buttons buttons;
    };

    static constexpr uint32_t key_f1 = SDLK_F1;
    static constexpr uint32_t key_f2 = SDLK_F2;
    static constexpr uint32_t mod_alt = KMOD_ALT;
    static constexpr uint32_t mod_gui = KMOD_GUI;
    static constexpr uint32_t mod_ctrl = KMOD_CTRL;
    static constexpr uint32_t mod_shift = KMOD_SHIFT;

    class input_binding {
    public:
        enum types {
            none = 1,
            key_combination,
            text_input,
            mouse,
            joystick
        };

        static input_binding for_text_input();

        static input_binding for_key_combination(const input_keys& keys);

        static input_binding for_joystick_buttons(const input_joystick_t& joystick);

        std::string text() const;

        bool matches(const SDL_Event* event) const;

    protected:
        explicit input_binding(input_binding::types type);

        inline bool is_modifier_key(input_key value) const {
            return value == mod_alt
                   || value == mod_ctrl
                   || value == mod_shift
                   || value == mod_gui;
        }

        inline bool is_modifier_pressed(input_key value) const {
            return (SDL_GetModState() & value) != 0;
        }

    private:
        types _type;
        std::string _text;
        input_keys _keys {};
        input_joystick_t _joystick {};
    };

};

