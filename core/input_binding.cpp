//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <map>
#include "joysticks.h"
#include "input_binding.h"

namespace ryu::core {

    struct keycode_to_ascii_t {
        int32_t mod;
        char ascii;
    };

    // XXX: ensure all specialized keycodes are added
    static std::map<int32_t, std::vector<keycode_to_ascii_t>> s_keycode_map = {
        {SDLK_ESCAPE,       {{KMOD_NONE, 0x1b}}},
        {SDLK_EXCLAIM,      {{KMOD_NONE, '!'}}},
        {SDLK_BACKQUOTE,    {{KMOD_NONE, '`'},  {KMOD_SHIFT, '~'}}},
        {SDLK_1,            {{KMOD_NONE, '1'},  {KMOD_SHIFT, '!'}}},
        {SDLK_2,            {{KMOD_NONE, '2'},  {KMOD_SHIFT, '@'}}},
        {SDLK_3,            {{KMOD_NONE, '3'},  {KMOD_SHIFT, '#'}}},
        {SDLK_4,            {{KMOD_NONE, '4'},  {KMOD_SHIFT, '$'}}},
        {SDLK_5,            {{KMOD_NONE, '5'},  {KMOD_SHIFT, '%'}}},
        {SDLK_PERCENT,      {{KMOD_NONE, '%'}}},
        {SDLK_6,            {{KMOD_NONE, '6'},  {KMOD_SHIFT, '^'}}},
        {SDLK_7,            {{KMOD_NONE, '7'},  {KMOD_SHIFT, '&'}}},
        {SDLK_8,            {{KMOD_NONE, '8'},  {KMOD_SHIFT, '*'}}},
        {SDLK_9,            {{KMOD_NONE, '9'},  {KMOD_SHIFT, '('}}},
        {SDLK_0,            {{KMOD_NONE, '0'},  {KMOD_SHIFT, ')'}}},
        {SDLK_MINUS,        {{KMOD_NONE, '-'},  {KMOD_SHIFT, '_'}}},
        {SDLK_EQUALS,       {{KMOD_NONE, '='},  {KMOD_SHIFT, '+'}}},
        {SDLK_TAB,          {{KMOD_NONE, '\t'}, {KMOD_SHIFT, '\t'}}},
        {SDLK_q,            {{KMOD_NONE, 'q'},  {KMOD_SHIFT, 'Q'}}},
        {SDLK_w,            {{KMOD_NONE, 'w'},  {KMOD_SHIFT, 'W'}}},
        {SDLK_e,            {{KMOD_NONE, 'e'},  {KMOD_SHIFT, 'E'}}},
        {SDLK_r,            {{KMOD_NONE, 'r'},  {KMOD_SHIFT, 'R'}}},
        {SDLK_t,            {{KMOD_NONE, 't'},  {KMOD_SHIFT, 'T'}}},
        {SDLK_y,            {{KMOD_NONE, 'y'},  {KMOD_SHIFT, 'Y'}}},
        {SDLK_u,            {{KMOD_NONE, 'u'},  {KMOD_SHIFT, 'U'}}},
        {SDLK_i,            {{KMOD_NONE, 'i'},  {KMOD_SHIFT, 'I'}}},
        {SDLK_o,            {{KMOD_NONE, 'o'},  {KMOD_SHIFT, 'O'}}},
        {SDLK_p,            {{KMOD_NONE, 'p'},  {KMOD_SHIFT, 'P'}}},
        {SDLK_LEFTBRACKET,  {{KMOD_NONE, '['},  {KMOD_SHIFT, '{'}}},
        {SDLK_RIGHTBRACKET, {{KMOD_NONE, ']'},  {KMOD_SHIFT, '}'}}},
        {SDLK_BACKSLASH,    {{KMOD_NONE, '\\'}, {KMOD_SHIFT, '|'}}},
        {SDLK_a,            {{KMOD_NONE, 'a'},  {KMOD_SHIFT, 'A'}}},
        {SDLK_s,            {{KMOD_NONE, 's'},  {KMOD_SHIFT, 'S'}}},
        {SDLK_d,            {{KMOD_NONE, 'd'},  {KMOD_SHIFT, 'D'}}},
        {SDLK_f,            {{KMOD_NONE, 'f'},  {KMOD_SHIFT, 'F'}}},
        {SDLK_g,            {{KMOD_NONE, 'g'},  {KMOD_SHIFT, 'G'}}},
        {SDLK_h,            {{KMOD_NONE, 'h'},  {KMOD_SHIFT, 'H'}}},
        {SDLK_j,            {{KMOD_NONE, 'j'},  {KMOD_SHIFT, 'J'}}},
        {SDLK_k,            {{KMOD_NONE, 'k'},  {KMOD_SHIFT, 'K'}}},
        {SDLK_l,            {{KMOD_NONE, 'l'},  {KMOD_SHIFT, 'L'}}},
        {SDLK_SEMICOLON,    {{KMOD_NONE, ';'},  {KMOD_SHIFT, ':'}}},
        {SDLK_QUOTE,        {{KMOD_NONE, '\''}, {KMOD_SHIFT, '"'}}},
        {SDLK_RETURN,       {{KMOD_NONE, '\n'}}},
        {SDLK_z,            {{KMOD_NONE, 'z'},  {KMOD_SHIFT, 'Z'}}},
        {SDLK_x,            {{KMOD_NONE, 'x'},  {KMOD_SHIFT, 'X'}}},
        {SDLK_c,            {{KMOD_NONE, 'c'},  {KMOD_SHIFT, 'C'}}},
        {SDLK_v,            {{KMOD_NONE, 'v'},  {KMOD_SHIFT, 'V'}}},
        {SDLK_b,            {{KMOD_NONE, 'b'},  {KMOD_SHIFT, 'B'}}},
        {SDLK_n,            {{KMOD_NONE, 'n'},  {KMOD_SHIFT, 'N'}}},
        {SDLK_m,            {{KMOD_NONE, 'm'},  {KMOD_SHIFT, 'M'}}},
        {SDLK_COMMA,        {{KMOD_NONE, ','},  {KMOD_SHIFT, '<'}}},
        {SDLK_PERIOD,       {{KMOD_NONE, '.'},  {KMOD_SHIFT, '>'}}},
        {SDLK_SLASH,        {{KMOD_NONE, '/'},  {KMOD_SHIFT, '?'}}},
        {SDLK_SPACE,        {{KMOD_NONE, ' '}}}
    };

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

    input_binding input_binding::for_joystick_hat(
            int32_t index,
            uint8_t hat_id,
            hat_state state) {
        auto binding = input_binding(types::joystick);
        binding._joystick = std::make_unique<input_joystick_t>(index, hat_id, state);
        return binding;
    }

    input_binding input_binding::for_text_input() {
        return input_binding(types::text_input);
    }

    input_binding input_binding::for_joystick_buttons(
            int32_t index,
            button_state buttons) {
        auto binding = input_binding(types::joystick);
        binding._joystick = std::make_unique<input_joystick_t>(index, buttons);
        return binding;
    }

    input_binding input_binding::for_key_combination(const input_keys& keys) {
        auto binding = input_binding(types::key_combination);
        binding._keys = keys;
        return binding;
    }

    input_binding::input_binding(input_binding::types type) : _type(type) {
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

                uint32_t mod = KMOD_NONE;
                auto key_total_count = 0;
                auto key_match_count = 0;

                for (auto key : _keys) {
                    if (is_modifier_key(key)) {
                        mod |= key;
                    } else {
                        key_total_count++;
                        if (event->key.keysym.sym == key)
                            key_match_count++;
                    }
                }

                if (event->key.keysym.mod == 0 && mod != 0)
                    return false;

                if (event->key.keysym.mod != 0 && (event->key.keysym.mod & mod) == 0)
                    return false;

                if (key_match_count == key_total_count) {
                    data.key_code = event->key.keysym.sym;
                    return true;
                }

                return false;
            }
            case text_input: {
                if (event->type != SDL_KEYDOWN)
                    return false;
                auto it = s_keycode_map.find(event->key.keysym.sym);
                if (it == s_keycode_map.end())
                    return false;
                if (it->second.size() == 1) {
                    data.c = it->second.front().ascii;
                    data.key_code = event->key.keysym.sym;
                } else {
                    // XXX: sometimes pressing ALT+<key> yields a character and sometimes not
                    for (auto mapping : it->second) {
                        if ((event->key.keysym.mod == KMOD_NONE && mapping.mod == KMOD_NONE)
                        ||  ((event->key.keysym.mod & mapping.mod) != 0)) {
                            data.c = mapping.ascii;
                            data.key_code = event->key.keysym.sym;
                        }
                    }
                }
                return true;
            }
            case mouse: {
                return false;
            }
            case joystick: {
                switch (event->type) {
                    case SDL_JOYBUTTONDOWN: {
                        auto joystick_device = joysticks::instance()->device_by_id(event->jbutton.which);
                        if (joystick_device == nullptr)
                            return false;
                        joystick_device->button(event->jbutton.button, true);
                        break;
                    }
                    case SDL_JOYBUTTONUP: {
                        auto joystick_device = joysticks::instance()->device_by_id(event->jbutton.which);
                        if (joystick_device == nullptr)
                            return false;
                        joystick_device->button(event->jbutton.button, false);
                        break;
                    }
                    case SDL_JOYHATMOTION: {
                        auto joystick_device = joysticks::instance()->device_by_id(event->jhat.which);
                        if (joystick_device == nullptr)
                            return false;
                        joystick_device->hat(event->jhat.hat, event->jhat.value);
                        break;
                    }
                    default: {
                        auto joystick_device = joysticks::instance()->device_by_id(_joystick->_index);
                        switch (_joystick->_type) {
                            case input_joystick_t::hat:
                                return (joystick_device->hat(_joystick->_hat_id) & _joystick->_hat_state) != 0;
                            case input_joystick_t::button:
                                return (joystick_device->buttons() & _joystick->_buttons) != 0;
                            case input_joystick_t::axis:
                            case input_joystick_t::ball:
                                break;
                        }
                    }
                }

                return false;
            }
            default:
                return false;
        }
    }

    input_keys input_binding::key_config() const {
        return _keys;
    }

    input_binding::types input_binding::type() const {
        return _type;
    }

    const input_joystick_t* input_binding::joystick_config() const {
        return _joystick.get();
    }

}