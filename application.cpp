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
#include <hardware/hardware.h>
#include "application.h"

namespace ryu {

    application::application() : _engine(display_width, display_height),
                                 _ide_context("ide"),
                                 _emulator_context("emulator") {
    }

    bool application::init(int argc, char** argv) {
        hardware::initialize();

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

        _engine.on_resize([&](const core::rect& bounds) {
            _ide_context.bounds(ide_bounds(bounds));
            _emulator_context.bounds(emulator_bounds(bounds));
        });

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
        if (!init(argc, argv))
            return 1;

        _engine.focus(_ide_context.id());

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
        _engine.add_context(&_ide_context);
        _ide_context.initialize(
                ide_bounds({0, 0, display_width, display_height}),
                ide::ide_context::colors::fill_color);
    }

    void application::configure_emulator() {
        _engine.add_context(&_emulator_context);
        _emulator_context.initialize(
                emulator_bounds({0, 0, display_width, display_height}),
                emulator::emulator_context::colors::fill_color);
    }

    core::rect application::ide_bounds(const core::rect& bounds) {
        return {
                0,
                0,
                (bounds.width() / 2) - 1,
                bounds.height()
        };
    }

    core::rect application::emulator_bounds(const core::rect& bounds) {
        return {
                (bounds.width() / 2) + 1,
                0,
                (bounds.width() / 2) - 1,
                bounds.height()
        };
    }

}