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

namespace ryu::core {

    class engine {
    public:
        using resize_callable = std::function<void (const core::rect&)>;

        engine(int width, int height);

        ~engine() = default;

        void quit();

        int focus() const {
            return _focused_context;
        }

        void focus(int id);

        short display_width() const;

        short display_height() const;

        bool run(core::result& result);

        bool initialize(result& result);

        hardware::machine* machine() const;

        bool shutdown(core::result& result);

        inline SDL_Renderer* renderer() const {
            return _renderer;
        }

        void add_context(core::context* context);

        void machine(hardware::machine* machine);

        void remove_context(core::context* context);

        void erase_blackboard(const std::string& name);

        void on_resize(const resize_callable& callable);

        std::string blackboard(const std::string& name) const;

        core::font_family* find_font_family(const std::string& name);

        void blackboard(const std::string& name, const std::string& value);

        core::font_family* add_font_family(uint32_t size, const std::string& name);

    private:
        core::rect bounds();

    private:
        bool _quit = false;
        int _focused_context = -1;
        core::blackboard _blackboard;
        core::context_dict _contexts;
        SDL_Window* _window = nullptr;
        core::font_t* _font = nullptr;
        font_family_dict _font_families;
        resize_callable _resize_callable;
        SDL_Renderer* _renderer = nullptr;
        std::pair<short, short> _display_size;
        hardware::machine* _machine = nullptr;
    };

};