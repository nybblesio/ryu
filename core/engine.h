//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <hardware/machine.h>
#include <common/SDL_FontCache.h>
#include "rect.h"
#include "timer.h"
#include "result.h"
#include "core_types.h"
#include "state_stack.h"
#include "font_family.h"
#include "preferences.h"

namespace ryu::core {

    class engine {
    public:
        using move_callable = std::function<void (const core::rect&)>;
        using resize_callable = std::function<void (const core::rect&)>;

        const uint16_t action_quit = 3;
        const uint16_t action_window_moved = 4;
        const uint16_t action_window_resized = 5;
        const uint16_t action_window_restore = 8;
        const uint16_t action_window_minimized = 6;
        const uint16_t action_window_maximized = 7;

        static std::vector<core::rect> displays();

        engine() = default;

        ~engine() = default;

        void quit();

        bool shutdown(
                core::result& result,
                core::preferences& prefs);

        bool initialize(
                result& result,
                const core::preferences& prefs);

        void blackboard(
                const std::string& name,
                const std::string& value);

        void raise_move();

        int focus() const {
            return _focused_context;
        }

        void focus(int id);

        void raise_resize();

        core::rect bounds() const;

        bool run(core::result& result);

        const core::font_t* font_face();

        core::font_family* font_family();

        core::rect window_position() const;

        hardware::machine* machine() const;

        inline SDL_Renderer* renderer() const {
            return _renderer;
        }

        void add_context(core::context* context);

        void machine(hardware::machine* machine);

        void font_face(const core::font_t* value);

        void font_family(core::font_family* value);

        void remove_context(core::context* context);

        void on_move(const move_callable& callback);

        void window_position(const core::rect& value);

        void erase_blackboard(const std::string& name);

        void on_resize(const resize_callable& callback);

        core::context* find_context(const std::string& name);

        std::string blackboard(const std::string& name) const;

    private:
        bool _quit = false;
        core::rect _window_rect;
        int _focused_context = -1;
        move_callable _move_callback;
        core::blackboard _blackboard;
        core::context_dict _contexts;
        SDL_Window* _window = nullptr;
        resize_callable _resize_callback;
        SDL_Renderer* _renderer = nullptr;
        const core::font_t* _font = nullptr;
        hardware::machine* _machine = nullptr;
        core::font_family* _font_family = nullptr;
    };

};