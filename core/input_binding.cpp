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

    bool input_binding::matches(const SDL_Event* event) const {
        switch(_type) {
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