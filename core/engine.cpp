//
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
#include "font_book.h"
#include "timer_pool.h"
#include "preferences.h"
#include "input_action.h"

namespace ryu::core {

    std::vector<rect> engine::displays() {
        std::vector<rect> displays;

        auto display_count = SDL_GetNumVideoDisplays();
        for (auto i = 0; i < display_count; i++) {
            SDL_Rect bounds{};
            SDL_GetDisplayBounds(i, &bounds);
            displays.emplace_back(bounds);
        }

        return displays;
    }

    void engine::quit() {
        _quit = true;
    }

    bool engine::shutdown(
            core::result& result,
            core::preferences& prefs) {
        prefs.window_position(_window_rect);
        if (_font_family != nullptr) {
            prefs.engine_font(std::make_pair(
                    _font_family->name(),
                    _font_family->size()));
        }

        if (_renderer != nullptr)
            SDL_DestroyRenderer(_renderer);

        if (_window != nullptr)
            SDL_DestroyWindow(_window);

        IMG_Quit();
        SDL_Quit();

        return !result.is_failed();
    }

    bool engine::initialize(
            core::result& result,
            const core::preferences& prefs) {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            result.add_message(
                    "R002",
                    "SDL_Init failed.",
                    fmt::format("SDL Error: {0}", SDL_GetError()),
                    true);
            return false;
        }

        _window_rect = prefs.window_position();
        _window = SDL_CreateWindow(
                "Ryu: The Arcade Construction Kit",
                _window_rect.left(),
                _window_rect.top(),
                _window_rect.width(),
                _window_rect.height(),
                SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
        if (_window == nullptr) {
            result.add_message(
                    "R003",
                    "SDL_CreateWindow failed.",
                    fmt::format("SDL Error: {0}", SDL_GetError()),
                    true);
            return false;
        }

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
            return false;
        }

        if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
            result.add_message(
                    "R005",
                    "IMG_Init failed.",
                    fmt::format("IMG Error: {0}", IMG_GetError()),
                    true);
            return false;
        }

        font_book::instance()->renderer(_renderer);
        font_book::instance()->load(result, prefs.font_book_path());

        const auto& engine_font = prefs.engine_font();
        auto family = font_book::instance()->find_font_family(
                engine_font.first,
                engine_font.second);
        if (family != nullptr) {
            font_family(family);
            auto face = family->find_style(font::styles::normal);
            if (face != nullptr) {
                font_face(face);
            }
        }

        auto quit_action = input_action::create(
            "ryu_quit",
            "Ryu",
            "Quit the Ryu application.");
        quit_action->register_handler(
            action_sink::engine,
            action_sink::default_filter,
            [this](const event_data_t& data) {
                quit();
                return true;
            });
        quit_action->bind_quit();

        auto minimized_action = input_action::create(
            "ryu_minimized",
            "Ryu",
            "Minimize the Ryu application window.");
        minimized_action->register_handler(
            action_sink::engine,
            action_sink::default_filter,
            [this](const event_data_t& data) {
                return true;
            });
        minimized_action->bind_minimized();

        auto maximized_action = input_action::create(
            "ryu_maximized",
            "Ryu",
            "Maximize the Ryu application window.");
        maximized_action->register_handler(
            action_sink::engine,
            action_sink::default_filter,
            [this](const event_data_t& data) {
                return true;
            });
        maximized_action->bind_maximized();

        auto move_action = input_action::create(
            "ryu_moved",
            "Ryu",
            "The Ryu application window was moved.");
        move_action->register_handler(
            action_sink::engine,
            action_sink::default_filter,
            [this](const event_data_t& data) {
                _window_rect.left(data.x);
                _window_rect.top(data.y);
                raise_move();
                return true;
            });
        move_action->bind_move();

        auto resize_action = input_action::create(
            "ryu_resized",
            "Ryu",
            "The Ryu application window was resized.");
        resize_action->register_handler(
            action_sink::engine,
            action_sink::default_filter,
            [this](const event_data_t& data) {
                _window_rect.width(data.width);
                _window_rect.height(data.height);
                raise_resize();
                return true;
            });
        resize_action->bind_resize();

        auto restore_action = input_action::create(
            "ryu_restore",
            "Ryu",
            "The Ryu application window was restored from minimized or maximized.");
        restore_action->register_handler(
            action_sink::engine,
            action_sink::default_filter,
            [this](const event_data_t& data) {
                // XXX:
                return true;
            });
        restore_action->bind_restore();

        return true;
    }

    void engine::blackboard(
            const std::string& name,
            const std::string& value) {
        _blackboard[name] = value;
    }

    void engine::raise_move() {
        if (_move_callback != nullptr) {
            _move_callback(_window_rect);
        }
    }

    void engine::focus(int id) {
        _focused_context = id;
    }

    void engine::raise_resize() {
        if (_resize_callback != nullptr) {
            _resize_callback(core::rect{
                    0,
                    0,
                    _window_rect.width(),
                    _window_rect.height()});
        }
    }

    core::rect engine::bounds() const {
        return {0, 0, _window_rect.width(), _window_rect.height()};
    }

    bool engine::run(core::result& result) {
        short average_fps = 0;
        short frame_count = 0;
        auto last_time = SDL_GetTicks();
        auto last_fps_time = last_time;

        // XXX: is this the best place to call this?
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

            event_list events {};
            for (size_t i = 0; i < 5; i++) {
                SDL_Event e {};
                if (!SDL_PollEvent(&e))
                    break;

                for (const auto& action : input_action::catalog())
                    action.process(&e);

                events.push_back(e);
            }

            for (auto& it : _contexts)
                it.second->update(dt, surface, events);

            events.clear();

            // N.B. this is an override/overlay draw so contexts
            //      can "bleed" into other contexts.
            for (auto& it : _contexts)
                it.second->draw(surface);

            surface.set_clip_rect(bounds());

            if (_font != nullptr) {
                auto fps = fmt::format("FPS: {}", (int) average_fps);
                auto fps_width = surface.measure_text(_font, fps);
                surface.draw_text(
                        _font,
                        _window_rect.width() - (fps_width + 5),
                        _window_rect.height() - (_font->line_height + 4),
                        fps,
                        {0xff, 0xff, 0xff, 0xff});
            }

            surface.present();

            ++frame_count;
            last_time = current_time;
        }

        return !result.is_failed();
    }

    const core::font_t* engine::font_face() {
        return _font;
    }

    core::font_family* engine::font_family() {
        return _font_family;
    }

    core::rect engine::window_position() const {
        return _window_rect;
    }

    hardware::machine* engine::machine() const {
        return _machine;
    }

    void engine::machine(hardware::machine* machine) {
        _machine = machine;
    }

    void engine::add_context(core::context* context) {
        if (context == nullptr)
            return;
        context->engine(this);
        _contexts.insert(std::make_pair(context->id(), context));
    }

    void engine::font_face(const core::font_t* value) {
        _font = value;
    }

    void engine::font_family(core::font_family* value) {
        _font_family = value;
    }

    void engine::remove_context(core::context* context) {
        if (context == nullptr)
            return;
        context->engine(nullptr);
        _contexts.erase(context->id());
    }

    void engine::window_position(const core::rect& value) {
        SDL_SetWindowPosition(_window, value.left(), value.top());
        SDL_SetWindowSize(_window, value.width(), value.height());
    }

    void engine::erase_blackboard(const std::string& name) {
        _blackboard.erase(name);
    }

    void engine::on_move(const engine::move_callable& callback) {
        _move_callback = callback;
    }

    core::context* engine::find_context(const std::string& name) {
        for (auto it = _contexts.begin();
             it != _contexts.end();
             ++it) {
            if (it->second->name() == name)
                return it->second;
        }
        return nullptr;
    }

    std::string engine::blackboard(const std::string& name) const {
        auto it = _blackboard.find(name);
        if (it != _blackboard.end()) {
            return it->second;
        }
        return "";
    }

    void engine::on_resize(const engine::resize_callable& callback) {
        _resize_callback = callback;
    }

}