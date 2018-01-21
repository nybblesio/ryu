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
        using resize_callable = std::function<void (const core::rect&)>;

        static std::vector<core::rect> displays();

        engine() = default;

        ~engine() = default;

        void quit();

        int focus() const {
            return _focused_context;
        }

        void focus(int id);

        core::rect bounds() const;

        bool run(core::result& result);

        core::rect window_position() const;

        hardware::machine* machine() const;

        bool shutdown(core::result& result);

        core::font_family* add_font_family(
                uint32_t size,
                const std::string& name);

        inline SDL_Renderer* renderer() const {
            return _renderer;
        }

        void add_context(core::context* context);

        void machine(hardware::machine* machine);

        void remove_context(core::context* context);

        void window_position(const core::rect& value);

        void erase_blackboard(const std::string& name);

        void on_resize(const resize_callable& callable);

        std::string blackboard(const std::string& name) const;

        core::font_family* find_font_family(const std::string& name);

        bool initialize(result& result, const core::preferences& prefs);

        void blackboard(const std::string& name, const std::string& value);

    private:
        bool _quit = false;
        core::rect _window_rect;
        int _focused_context = -1;
        core::blackboard _blackboard;
        core::context_dict _contexts;
        SDL_Window* _window = nullptr;
        core::font_t* _font = nullptr;
        font_family_dict _font_families;
        resize_callable _resize_callable;
        SDL_Renderer* _renderer = nullptr;
        hardware::machine* _machine = nullptr;
    };

};