//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <iostream>
#include "ryu_types.h"
#include "application.h"

namespace ryu {

    application::application() : _engine(display_width, display_height),
                                 _ide_context(app::contexts::ide, "ide"),
                                 _emulator_context(app::contexts::emulator, "emulator") {
    }

    bool application::init() {
        core::result result;

        if (!_engine.initialize(result)) {
            std::cout << "initialize failed: " << std::endl;
            for (auto& message : result.messages())
                std::cout << message.code() << ": "
                          << message.message() << std::endl
                          << "\t" << message.details() << std::endl;
            return false;
        }

        configure_ide();
        configure_emulator();

        return true;
    }

    bool application::shutdown() {
        core::result result;

        if (!_engine.shutdown(result)) {
            std::cout << "shutdown failed: " << std::endl;
            for (auto& message : result.messages())
                std::cout << message.code() << ": "
                          << message.message() << std::endl
                          << "\t" << message.details() << std::endl;
            return false;
        }

        return true;
    }

    int application::run(int argc, char** argv) {
        if (!init())
            return 1;

        _engine.focus(app::contexts::ide);

        core::result result;

        if (!_engine.run(result)) {
            std::cout << "run failed: " << std::endl;
            for (auto& message : result.messages())
                std::cout << message.code() << ": "
                          << message.message() << std::endl
                          << "\t" << message.details() << std::endl;
            return 1;
        }

        return !shutdown() ? 1 : 0;
    }

    void application::configure_ide() {
        SDL_Rect rect = {
                0,
                0,
                (display_width / 2) - 1,
                display_height
        };

        _ide_context.initialize(&_engine, rect, ide::context::colors::fill_color);
        _ide_context.push_state(ide::context::states::console);
        _engine.add_context(&_ide_context);
    }

    void application::configure_emulator() {
        SDL_Rect rect = {
                (display_width / 2) + 1,
                0,
                (display_width / 2) - 1,
                display_height
        };

        _emulator_context.initialize(&_engine, rect, emulator::context::colors::fill_color);
        _emulator_context.push_state(emulator::context::states::execute);
        _engine.add_context(&_emulator_context);
    }

}