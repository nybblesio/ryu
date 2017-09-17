
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include <algorithm>
#include <fmt/format.h>
#include <common/string_support.h>
#include "state.h"
#include "engine.h"
#include "context.h"

namespace ryu::core {

    engine::engine(int width, int height) : _display_size(std::make_pair(width, height)) {
    }

    void engine::quit() {
        _quit = true;
    }

    font_t* engine::load_font(
            const std::string& name,
            const std::string& path,
            uint32_t size,
            int style) {
        auto font = find_font(name);
        if (font != nullptr)
            return font;

        auto instance = FC_CreateFont();
        auto result = FC_LoadFont(
                instance,
                _renderer,
                path.c_str(),
                size,
                FC_MakeColor(0xff, 0xff, 0xff, 0xff),
                style);
        if (result != 0) {
            auto it = _fonts.insert(std::make_pair(
                    name,
                    font_t{FC_GetWidth(instance, "A"), size, FC_GetLineHeight(instance), instance}));
            return &(*it.first).second;
        }
        return nullptr;
    }

    void engine::focus(int id) {
        _focused_context = id;
    }

    short engine::display_width() const {
        return _display_size.first;
    }

    short engine::display_height() const {
        return _display_size.second;
    }

    bool engine::run(core::result& result) {
        short average_fps = 0;
        short frame_count = 0;
        auto last_time = SDL_GetTicks();
        auto last_fps_time = last_time;
        SDL_Rect clip{0, 0, _display_size.first, _display_size.second};
        SDL_StartTextInput();

        while (!_quit) {
            auto current_time = SDL_GetTicks();
            auto dt = current_time - last_time;

            auto fps_dt = last_time - last_fps_time;
            if (fps_dt >= 1000) {
                average_fps = frame_count;
                frame_count = 0;
                last_fps_time = last_time;
            }

            SDL_SetRenderDrawColor(_renderer, 0x00, 0x00, 0x00, 0xff);
            SDL_RenderClear(_renderer);

            for (auto& it : _contexts)
                it.second->update(dt);

            SDL_RenderSetClipRect(_renderer, &clip);
            FC_SetDefaultColor(_font->glyph, FC_MakeColor(0xff, 0xff, 0xff, 0xff));
            auto fps = fmt::format("FPS: {}", (int) average_fps);
            int fps_width = FC_GetWidth(_font->glyph, fps.c_str());
            FC_Draw(
                    _font->glyph,
                    _renderer,
                    display_width() - (fps_width + 5),
                    display_height() - (_font->line_height + 4),
                    fps.c_str());

            SDL_RenderPresent(_renderer);

            ++frame_count;
            last_time = current_time;
        }

        return !result.is_failed();
    }

    hardware::machine* engine::machine() const {
        return _machine;
    }

    bool engine::shutdown(core::result& result) {
        for (auto& it : _fonts) {
            if (it.second.glyph != nullptr)
                FC_FreeFont(it.second.glyph);
        }
        _fonts.clear();

        if (_renderer != nullptr)
            SDL_DestroyRenderer(_renderer);

        if (_window != nullptr)
            SDL_DestroyWindow(_window);

        IMG_Quit();
        SDL_Quit();

        return !result.is_failed();
    }

    bool engine::initialize(core::result& result) {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            result.add_message(
                    "R002",
                    "SDL_Init failed.",
                    fmt::format("SDL Error: {0}", SDL_GetError()),
                    true);
            result.fail();
        } else {
            _window = SDL_CreateWindow(
                    "Ryu: The Arcade Construction Kit",
                    SDL_WINDOWPOS_CENTERED,
                    SDL_WINDOWPOS_CENTERED,
                    display_width(),
                    display_height(),
                    SDL_WINDOW_SHOWN);
            if (_window == nullptr) {
                result.add_message(
                        "R003",
                        "SDL_CreateWindow failed.",
                        fmt::format("SDL Error: {0}", SDL_GetError()),
                        true);
                result.fail();
            } else {
                _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
                if (_renderer == nullptr) {
                    result.add_message(
                            "R004",
                            "SDL_CreateRenderer failed.",
                            fmt::format("SDL Error: {0}", SDL_GetError()),
                            true);
                    result.fail();
                } else {
                    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
                        result.add_message(
                                "R005",
                                "IMG_Init failed.",
                                fmt::format("IMG Error: {0}", IMG_GetError()),
                                true);
                        result.fail();
                    } else {
                        _font = load_font("hack-normal", "assets/Hack-Regular.ttf", 16, TTF_STYLE_NORMAL);
                        load_font("hack-bold",      "assets/Hack-Bold.ttf",         16, TTF_STYLE_NORMAL);
                        load_font("hack-italic",    "assets/Hack-Italic.ttf",       16, TTF_STYLE_ITALIC);
                        load_font("hack-underline", "assets/Hack-Regular.ttf",      16, TTF_STYLE_UNDERLINE);

                        load_font("topaz-normal", "assets/Topaz-8.ttf", 14, TTF_STYLE_NORMAL);
                    }
                }
            }
        }

        return !result.is_failed();
    }

    void engine::add_context(core::context* context) {
        if (context == nullptr)
            return;
        _contexts.insert(std::make_pair(context->id(), context));
    }

    void engine::machine(hardware::machine* machine) {
        _machine = machine;
    }

    void engine::remove_context(core::context* context) {
        if (context == nullptr)
            return;
        _contexts.erase(context->id());
    }

    font_t* engine::find_font(const std::string& name) {
        auto it = _fonts.find(name);
        if (it != _fonts.end())
            return &it->second;
        return nullptr;
    }

    void engine::erase_blackboard(const std::string& name) {
        _blackboard.erase(name);
    }

    std::string engine::blackboard(const std::string& name) const {
        auto it = _blackboard.find(name);
        if (it != _blackboard.end()) {
            return it->second;
        }
        return "";
    }

    void engine::blackboard(const std::string& name, const std::string& value) {
        _blackboard[name] = value;
    }

}