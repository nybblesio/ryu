//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "input_binding.h"

namespace ryu::core {

    input_binding input_binding::for_quit() {
        return input_binding(types::quit);
    }

    input_binding input_binding::for_move() {
        return input_binding(types::window_move);
    }

    input_binding input_binding::for_resize() {
        return input_binding(types::window_resize);
    }

    input_binding input_binding::for_restore() {
        return input_binding(types::window_restore);
    }

    input_binding input_binding::for_minimize() {
        return input_binding(types::window_minimized);
    }

    input_binding input_binding::for_maximize() {
        return input_binding(types::window_maximized);
    }

    input_binding input_binding::for_text_input() {
        return input_binding(types::text_input);
    }

    input_binding input_binding::for_key_combination(const input_keys& keys) {
        auto binding = input_binding(types::key_combination);
        binding._keys = keys;
        return binding;
    }

    input_binding input_binding::for_joystick_buttons(const input_joystick_t& joystick) {
        auto binding = input_binding(types::joystick);
        binding._joystick = joystick;
        return binding;
    }

    input_binding::input_binding(input_binding::types type) : _type(type) {
    }

    std::string input_binding::text() const {
        return _text;
    }

    bool input_binding::matches(
            const SDL_Event* event,
            event_data_t& data) const {
        switch(_type) {
            case quit: {
                return event->type == SDL_QUIT;
            }
            case window_minimized: {
                auto flag = event->type == SDL_WINDOWEVENT
                            && event->window.event == SDL_WINDOWEVENT_MINIMIZED;
                if (flag) {
                    data.x = 0;
                    data.y = 0;
                    data.width = 0;
                    data.height = 0;
                }
                return flag;
            }
            case window_maximized: {
                auto flag = event->type == SDL_WINDOWEVENT
                            && event->window.event == SDL_WINDOWEVENT_MAXIMIZED;
                if (flag) {
                    data.x = 0;
                    data.y = 0;
                    data.width = event->window.data1;
                    data.height = event->window.data2;
                }
                return flag;
            }
            case window_move: {
                auto flag = event->type == SDL_WINDOWEVENT
                            && event->window.event == SDL_WINDOWEVENT_MOVED;
                if (flag) {
                    data.x = event->window.data1;
                    data.y = event->window.data2;
                }
                return flag;
            }
            case window_resize: {
                auto flag = event->type == SDL_WINDOWEVENT
                            && event->window.event == SDL_WINDOWEVENT_RESIZED;
                if (flag) {
                    data.width = event->window.data1;
                    data.height = event->window.data2;
                }
                return flag;
            }
            case window_restore: {
                return event->type == SDL_WINDOWEVENT
                       && event->window.event == SDL_WINDOWEVENT_RESTORED;
            }
            case key_combination: {
                if (event->type != SDL_KEYDOWN)
                    return false;
                for (auto key : _keys) {
                    if (is_modifier_key(key)) {
                        if (!is_modifier_pressed(key))
                            return false;
                    } else {
                        if (event->key.keysym.sym != key) {
                            return false;
                        }
                    }
                }
                return true;
            }
            case text_input: {
                return event->type == SDL_TEXTINPUT;
            }
            case mouse: {
                return false;
            }
            case joystick: {
                return false;
            }
            default:
                return false;
        }
    }

}