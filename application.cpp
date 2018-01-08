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
#include <ide/ide_types.h>
#include <hardware/hardware.h>
#include "application.h"

namespace ryu {

    application::application() : _engine(display_width, display_height),
                                 _ide_context("ide"),
                                 _emulator_context("emulator") {
    }

    void application::show_result_messages(core::result& result) {
        for (auto& message : result.messages())
            std::cout << message.code() << ": "
                      << message.message() << "\n"
                      << "\t" << message.details() << "\n";
    }

    bool application::init(int argc, char** argv) {
        hardware::initialize();

        core::result result;

        if (!_engine.initialize(result)) {
            std::cout << "initialize failed:\n";
            show_result_messages(result);
            return false;
        }

        if (!configure_ide(result)) {
            std::cout << "configure_ide failed:\n";
            show_result_messages(result);
            return false;
        }

        if (!configure_emulator(result)) {
            std::cout << "configure_emulator failed:\n";
            show_result_messages(result);
            return false;
        }

        _engine.on_resize([&](const core::rect& bounds) {
            _ide_context.bounds(ide_bounds(bounds));
            _emulator_context.bounds(emulator_bounds(bounds));
        });

        std::cout << std::endl;

        return true;
    }

    bool application::shutdown() {
        core::result result;

        if (!_engine.shutdown(result)) {
            std::cout << "shutdown failed:\n";
            show_result_messages(result);
            return false;
        }

        std::cout << std::endl;

        return true;
    }

    int application::run(int argc, char** argv) {
        if (!init(argc, argv))
            return 1;

        _engine.focus(_ide_context.id());

        core::result result;

        if (!_engine.run(result)) {
            std::cout << "run failed:\n";
            show_result_messages(result);
            return 1;
        }

        std::cout << std::endl;

        return !shutdown() ? 1 : 0;
    }

    bool application::configure_ide(core::result& result) {
        _engine.add_context(&_ide_context);
        _ide_context.bg_color(ide::colors::fill_color);
        return _ide_context.initialize(
                result,
                ide_bounds({0, 0, display_width, display_height}));
    }

    bool application::configure_emulator(core::result& result) {
        _engine.add_context(&_emulator_context);
        _emulator_context.bg_color(emulator::emulator_context::colors::fill_color);
        return _emulator_context.initialize(
                result,
                emulator_bounds({0, 0, display_width, display_height}));
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