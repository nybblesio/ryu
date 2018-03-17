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
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include "application.h"
#include "logger.h"

namespace ryu {

    void application::show_result_messages(core::result& result) {
        for (auto& message : result.messages()) {
            _log->error(fmt::format("{}: {}", message.code(), message.message()));
            if (!message.details().empty())
                _log->error(message.details());
        }
    }

    bool application::init(int argc, char** argv) {
        core::result result;

        log4cpp::PropertyConfigurator::configure("ryu.properties");
        _log = logger::instance()->category("ryu", log4cpp::Priority::INFO);
        _log->info("init start.");

        _executable_path = boost::filesystem::system_complete(argv[0]);
        _executable_path = _executable_path.parent_path();
        _prefs.executable_path(_executable_path);

        _log->info(fmt::format("executable path: {}", _executable_path.string()));

        _log->info("preferences load");
        if (!_prefs.load(result)) {
            _log->error("unable to load preferences");
            show_result_messages(result);
            return false;
        }

        _log->info("hardware initialize");
        if (!hardware::initialize(result, _executable_path)) {
            _log->error("hardware initialization failed:");
            show_result_messages(result);
            return false;
        }

        _log->info("engine initialize");
        if (!_engine.initialize(result, _prefs)) {
            _log->error("engine initialization failed:");
            show_result_messages(result);
            return false;
        }

        _log->info("ide configuration");
        if (!configure_ide(result)) {
            _log->error("ide configuration failed:");
            show_result_messages(result);
            return false;
        }

        _log->info("emulator configuration");
        if (!configure_emulator(result)) {
            _log->error("emulator initialization failed:");
            show_result_messages(result);
            return false;
        }

        _engine.on_resize([&](const core::rect& bounds) {
            _ide_context.parent_resize(bounds);
            _emulator_context.parent_resize(bounds);
        });

        return true;
    }

    bool application::shutdown() {
        core::result result;

        _log->info("shutdown start");


        _prefs.default_path(boost::filesystem::current_path());
        _log->info(fmt::format(
            "preferences default path: {}",
            _prefs.default_path().string()));

        _log->info("engine shutdown");
        if (!_engine.shutdown(result, _prefs)) {
            _log->error("engine shutdown failed:");
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

        _prefs.ide_window_size(_ide_context.size());
        _prefs.emulator_window_size(_emulator_context.size());

        _log->info("save preferences");
        if (!_prefs.save(result)) {
            _log->error("preferences save failed:");
            show_result_messages(result);
            return false;
        }

        return true;
    }

    int application::run(int argc, char** argv) {
        if (!init(argc, argv))
            return 1;

        _engine.focus(_ide_context.id());

        core::result result;

        _log->info("engine run");
        if (!_engine.run(result)) {
            _log->error("engine run failed:");
            show_result_messages(result);
            return 1;
        }

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
        _ide_context.size(_prefs.ide_window_size());
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
        _emulator_context.bg_color(ide::colors::fill_color);
        _emulator_context.size(_prefs.emulator_window_size());
        return _emulator_context.initialize(result, _engine.bounds());
    }

}