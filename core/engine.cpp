
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
#include "timer_pool.h"

namespace ryu::core {

    engine::engine(int width, int height) : _display_size(std::make_pair(width, height)) {
    }

    void engine::quit() {
        _quit = true;
    }

    void engine::focus(int id) {
        _focused_context = id;
    }

    core::rect engine::bounds() {
        return {0, 0, _display_size.first, _display_size.second};
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
        SDL_StartTextInput();

        core::renderer surface {_renderer};

        while (!_quit) {
            timer_pool::instance()->update();

            auto current_time = SDL_GetTicks();
            auto dt = current_time - last_time;

            auto fps_dt = last_time - last_fps_time;
            if (fps_dt >= 1000) {
                average_fps = frame_count;
                frame_count = 0;
                last_fps_time = last_time;
            }

            surface.set_color({0x00, 0x00, 0x00, 0xff});
            surface.clear();

            std::deque<SDL_Event> events;
            SDL_Event e {};
            while (SDL_PollEvent(&e) != 0) {
                switch (e.type) {
                    case SDL_WINDOWEVENT:
                        switch (e.window.event) {
                            case SDL_WINDOWEVENT_RESIZED:
                                _display_size = std::make_pair(e.window.data1, e.window.data2);
                                if (_resize_callable != nullptr) {
                                    _resize_callable(core::rect{0, 0, _display_size.first, _display_size.second});
                                }
                                break;
                        }
                        break;
                    case SDL_QUIT:
                        _quit = true;
                        continue;
                    default:
                        events.push_back(e);
                        break;
                }
            }

            for (auto& it : _contexts)
                it.second->update(dt, surface, events);

            surface.set_clip_rect(bounds());
            auto fps = fmt::format("FPS: {}", (int) average_fps);
            auto fps_width = surface.measure_text(_font, fps);
            surface.draw_text(
                    _font,
                    display_width() - (fps_width + 5),
                    display_height() - (_font->line_height + 4),
                    fps,
                    {0xff, 0xff, 0xff, 0xff});

            surface.present();

            ++frame_count;
            last_time = current_time;
        }

        return !result.is_failed();
    }

    hardware::machine* engine::machine() const {
        return _machine;
    }

    bool engine::shutdown(core::result& result) {
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
                    SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
            if (_window == nullptr) {
                result.add_message(
                        "R003",
                        "SDL_CreateWindow failed.",
                        fmt::format("SDL Error: {0}", SDL_GetError()),
                        true);
                result.fail();
            } else {
                SDL_SetWindowOpacity(_window, 1.0f);
                _renderer = SDL_CreateRenderer(
                        _window,
                        -1,
                        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
                        // XXX: need to load multiple sizes for user config
                        auto topaz = add_font_family(24, "topaz");
                        _font = topaz->add_style(font::styles::normal, "assets/Topaz-8.ttf");

                        auto hack = add_font_family(26, "hack");
                        hack->add_style(font::styles::normal,                      "assets/Hack-Regular.ttf");
                        hack->add_style(font::styles::bold,                        "assets/Hack-Bold.ttf");
                        // XXX: there appears to be an issue with this font
                        hack->add_style(font::styles::italic,                      "assets/Hack-Italic.ttf");
                        hack->add_style(font::styles::underline,                   "assets/Hack-Regular.ttf");
                        hack->add_style(font::styles::bold|font::styles::underline,"assets/Hack-Bold.ttf");
                        hack->add_style(font::styles::bold|font::styles::italic,   "assets/Hack-BoldItalic.ttf");
                    }
                }
            }
        }

        return !result.is_failed();
    }

    void engine::add_context(core::context* context) {
        if (context == nullptr)
            return;
        context->engine(this);
        _contexts.insert(std::make_pair(context->id(), context));
    }

    void engine::machine(hardware::machine* machine) {
        _machine = machine;
    }

    void engine::remove_context(core::context* context) {
        if (context == nullptr)
            return;
        context->engine(nullptr);
        _contexts.erase(context->id());
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

    void engine::on_resize(const engine::resize_callable& callable) {
        _resize_callable = callable;
    }

    core::font_family* engine::find_font_family(const std::string& name) {
        auto it = _font_families.find(name);
        if (it == _font_families.end())
            return nullptr;
        return &it->second;
    }

    void engine::blackboard(const std::string& name, const std::string& value) {
        _blackboard[name] = value;
    }

    core::font_family* engine::add_font_family(uint32_t size, const std::string& name) {
        auto it = _font_families.insert(std::make_pair(name, font_family(name, size, _renderer)));
        return &(*it.first).second;
    }

}