//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <hardware/machine.h>
#include "rect.h"
#include "timer.h"
#include "result.h"
#include "core_types.h"
#include "state_stack.h"
#include "font_family.h"
#include "preferences.h"
#include "input_action_provider.h"

struct SDL_Window;
struct SDL_Renderer;

namespace ryu::core {

    class engine {
    public:
        using move_callable = std::function<void (const core::rect&)>;
        using resize_callable = std::function<void (const core::rect&)>;

        static std::vector<core::rect> displays();

        engine() = default;

        ~engine() = default;

        void quit();

        bool shutdown(
                core::result& result,
                core::preferences& prefs);

        bool initialize(
                result& result,
                core::preferences* prefs);

        void raise_move();

        int focus() const {
            return _focused_context;
        }

        void focus(int id);

        void raise_resize();

        core::preferences* prefs();

        bool run(core::result& result);

        const core::font_t* font_face();

        core::font_family* font_family();

        core::rect window_position() const;

        void add_context(core::context* context);

        inline const core::rect& bounds() const {
            return _window_rect;
        }

        void font_face(const core::font_t* value);

        void font_family(core::font_family* value);

        void remove_context(core::context* context);

        void on_move(const move_callable& callback);

        inline const core::rect& clip_rect() const {
            return _clip_rect;
        }

        void window_position(const core::rect& value);

        void on_resize(const resize_callable& callback);

        core::context* find_context(const std::string& name);

    private:
        void bind_events();

        void define_actions();

    private:
        bool _quit = false;
        core::rect _clip_rect;
        core::rect _window_rect;
        core::renderer _surface;
        int _focused_context = -1;
        move_callable _move_callback;
        core::context_dict _contexts;
        SDL_Window* _window = nullptr;
        resize_callable _resize_callback;
        SDL_Renderer* _renderer = nullptr;
        core::preferences* _prefs = nullptr;
        const core::font_t* _font = nullptr;
        core::font_family* _font_family = nullptr;
        core::input_action_provider _action_provider {};
    };

};