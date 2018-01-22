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
#include <core/font_book.h>
#include <hardware/hardware.h>
#include <hardware/registry.h>
#include "application.h"

namespace ryu {

    void application::show_result_messages(core::result& result) {
        for (auto& message : result.messages())
            std::cout << message.code() << ": "
                      << message.message() << "\n"
                      << "\t" << message.details() << "\n";
    }

    bool application::init(int argc, char** argv) {
        core::result result;

        _executable_path = boost::filesystem::system_complete(argv[0]);
        _executable_path = _executable_path.parent_path();
        _prefs.executable_path(_executable_path);

        if (!_prefs.load(result)) {
            std::cout << "loading preferences failed:\n";
            show_result_messages(result);
            return false;
        }

        if (!hardware::initialize(result, _executable_path)) {
            std::cout << "hardware initialize failed:\n";
            show_result_messages(result);
            return false;
        }

        if (!_engine.initialize(result, _prefs)) {
            std::cout << "engine initialize failed:\n";
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
            _ide_context.parent_resize(bounds);
            _emulator_context.parent_resize(bounds);
        });

        std::cout << std::endl;

        return true;
    }

    bool application::shutdown() {
        core::result result;

        _prefs.default_path(boost::filesystem::current_path());

        if (!_engine.shutdown(result, _prefs)) {
            std::cout << "engine shutdown failed:\n";
            show_result_messages(result);
            return false;
        }

        auto ide_font_family = _ide_context.font_family();
        if (ide_font_family != nullptr) {
            _prefs.ide_font(std::make_pair(
                    ide_font_family->name(),
                    ide_font_family->size()));
        }

        auto emulator_font_family = _emulator_context.font_family();
        if (emulator_font_family != nullptr) {
            _prefs.emulator_font(std::make_pair(
                    emulator_font_family->name(),
                    emulator_font_family->size()));
        }

        if (!_prefs.save(result)) {
            std::cout << "saving preferences failed:\n";
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

        const auto& ide_font = _prefs.ide_font();
        auto family = core::font_book::instance()->find_font_family(
                ide_font.first,
                ide_font.second);
        if (family == nullptr) {
            result.add_message(
                    "R011",
                    "Unable to find ide font family.",
                    fmt::format("font family: {} {}",
                                ide_font.first,
                                ide_font.second),
                    true);
            return false;
        }

        _ide_context.font_family(family);
        auto face = family->find_style(core::font::styles::normal);
        if (face == nullptr) {
            result.add_message(
                    "R011",
                    "Unable to find ide font face.",
                    true);
            return false;
        }

        _ide_context.font_face(face);
        _ide_context.bg_color(ide::colors::fill_color);
        return _ide_context.initialize(result, _engine.bounds());
    }

    bool application::configure_emulator(core::result& result) {
        _engine.add_context(&_emulator_context);

        const auto& emulator_font = _prefs.emulator_font();
        auto family = core::font_book::instance()->find_font_family(
                emulator_font.first,
                emulator_font.second);
        if (family == nullptr) {
            result.add_message(
                    "R011",
                    "Unable to find emulator font family.",
                    fmt::format("font family: {} {}",
                                emulator_font.first,
                                emulator_font.second),
                    true);
            return false;
        }

        _emulator_context.font_family(family);
        auto face = family->find_style(core::font::styles::normal);
        if (face == nullptr) {
            result.add_message(
                    "R011",
                    "Unable to find emulator font face.",
                    true);
            return false;
        }

        _emulator_context.font_face(face);
        _emulator_context.bg_color(emulator::emulator_context::colors::fill_color);
        return _emulator_context.initialize(result, _engine.bounds());
    }

}