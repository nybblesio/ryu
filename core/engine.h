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
#include "timer.h"
#include "result.h"
#include "core_types.h"
#include "state_stack.h"

namespace ryu::core {

    class engine {
        friend class context;
    public:
        engine(int width, int height);

        ~engine() = default;

        void quit();

        void focus(int id);

        font_t* load_font(
                const std::string& name,
                const std::string& path,
                uint32_t size,
                int style);

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

        font_t* find_font(const std::string& name);

        void remove_context(core::context* context);

        void erase_blackboard(const std::string& name);

        std::string blackboard(const std::string& name) const;

        void blackboard(const std::string& name, const std::string& value);

    private:
        font_t* _font;
        font_dict _fonts;
        bool _quit = false;
        int _focused_context = -1;
        core::blackboard _blackboard;
        core::context_dict _contexts;
        SDL_Window* _window = nullptr;
        SDL_Renderer* _renderer = nullptr;
        std::pair<short, short> _display_size;
        hardware::machine* _machine = nullptr;
    };

};