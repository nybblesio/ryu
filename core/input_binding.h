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
#include "joystick.h"

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

    using joystick_buttons = uint32_t;

    struct input_joystick_t {
        enum types {
            hat = 1,
            button,
            axis,
            ball
        };

        input_joystick_t(
                uint32_t index,
                uint8_t id,
                hat_state state) : _type(types::hat),
                                   _index(index),
                                   _hat_id(id),
                                   _hat_state(state),
                                   _buttons(0) {
        }

        input_joystick_t(
                uint32_t index,
                button_state buttons) : _type(types::button),
                                        _index(index),
                                        _hat_id(0),
                                        _hat_state(0),
                                        _buttons(buttons) {
        }

        types _type;
        uint32_t _index;

        uint8_t _hat_id;
        hat_state _hat_state;

        button_state _buttons;
    };

    struct event_data_t {
        char c;
        int32_t x;
        int32_t y;
        int32_t width;
        int32_t height;
        int32_t key_code;
    };

    struct pending_event_t {
        uint32_t id;
        event_data_t data;
    };

    using pending_event_list = std::vector<pending_event_t>;

    static constexpr uint32_t key_0 = SDLK_0;
    static constexpr uint32_t key_1 = SDLK_1;
    static constexpr uint32_t key_2 = SDLK_2;
    static constexpr uint32_t key_3 = SDLK_3;
    static constexpr uint32_t key_4 = SDLK_4;
    static constexpr uint32_t key_5 = SDLK_5;
    static constexpr uint32_t key_6 = SDLK_6;
    static constexpr uint32_t key_7 = SDLK_7;
    static constexpr uint32_t key_8 = SDLK_8;
    static constexpr uint32_t key_9 = SDLK_9;
    static constexpr uint32_t key_f1 = SDLK_F1;
    static constexpr uint32_t key_f2 = SDLK_F2;
    static constexpr uint32_t key_f3 = SDLK_F3;
    static constexpr uint32_t key_f4 = SDLK_F4;
    static constexpr uint32_t key_f5 = SDLK_F5;
    static constexpr uint32_t key_f6 = SDLK_F6;
    static constexpr uint32_t key_f7 = SDLK_F7;
    static constexpr uint32_t key_f8 = SDLK_F8;
    static constexpr uint32_t key_f9 = SDLK_F9;
    static constexpr uint32_t key_f10 = SDLK_F10;
    static constexpr uint32_t key_f11 = SDLK_F11;
    static constexpr uint32_t key_f12 = SDLK_F12;
    static constexpr uint32_t key_up = SDLK_UP;
    static constexpr uint32_t key_c = SDLK_c;
    static constexpr uint32_t key_v = SDLK_v;
    static constexpr uint32_t key_x = SDLK_x;
    static constexpr uint32_t key_z = SDLK_z;
    static constexpr uint32_t key_y = SDLK_y;
    static constexpr uint32_t key_down = SDLK_DOWN;
    static constexpr uint32_t key_right = SDLK_RIGHT;
    static constexpr uint32_t key_left = SDLK_LEFT;
    static constexpr uint32_t key_home = SDLK_HOME;
    static constexpr uint32_t key_end = SDLK_END;
    static constexpr uint32_t key_tab = SDLK_TAB;
    static constexpr uint32_t key_escape = SDLK_ESCAPE;
    static constexpr uint32_t key_delete = SDLK_DELETE;
    static constexpr uint32_t key_backspace = SDLK_BACKSPACE;
    static constexpr uint32_t key_insert = SDLK_INSERT;
    static constexpr uint32_t key_space = SDLK_SPACE;
    static constexpr uint32_t key_return = SDLK_RETURN;
    static constexpr uint32_t key_page_up = SDLK_PAGEUP;
    static constexpr uint32_t key_page_down = SDLK_PAGEDOWN;
    static constexpr uint32_t mod_alt = KMOD_ALT;
    static constexpr uint32_t mod_gui = KMOD_GUI;
    static constexpr uint32_t mod_ctrl = KMOD_CTRL;
    static constexpr uint32_t mod_shift = KMOD_SHIFT;

    static constexpr uint8_t ascii_escape = 0x1b;
    static constexpr uint8_t ascii_return = '\n';
    static constexpr uint8_t ascii_tab = '\t';

    class input_binding {
    public:
        enum types {
            none = 1,
            key_combination,
            text_input,
            mouse,
            joystick,
            quit,
            window_minimized,
            window_maximized,
            window_resize,
            window_move,
            window_restore
        };

        static input_binding for_quit();

        static input_binding for_move();

        static input_binding for_resize();

        static input_binding for_restore();

        static input_binding for_minimize();

        static input_binding for_maximize();

        static input_binding for_text_input();

        static input_binding for_joystick_hat(
            int32_t index,
            uint8_t hat_id,
            hat_state state);

        static input_binding for_joystick_buttons(
            int32_t index,
            button_state buttons);

        static input_binding for_key_combination(const input_keys& keys);

        bool matches(
            const SDL_Event* event,
            event_data_t& data) const;

        types type() const;

        input_keys key_config() const;

        const input_joystick_t* joystick_config() const;

    protected:
        explicit input_binding(input_binding::types type);

        inline bool is_modifier_key(input_key value) const {
            return value == mod_alt
                   || value == mod_ctrl
                   || value == mod_shift
                   || value == mod_gui;
        }

    private:
        types _type;
        input_keys _keys {};
        std::unique_ptr<input_joystick_t> _joystick;
    };

};