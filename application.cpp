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
#include "logger_factory.h"

namespace ryu {

    static logger* s_log;

    bool application::init(int argc, char** argv) {
        core::result result;

        logger_factory::instance()->initialize("ryu.properties");
        s_log = logger_factory::instance()->create("ryu", logger::level::info);
        s_log->info("init start.");

        _executable_path = boost::filesystem::system_complete(argv[0]);
        _executable_path = _executable_path.parent_path();
        _prefs.executable_path(_executable_path);

        s_log->info(fmt::format("executable path: {}", _executable_path.string()));

        s_log->info("preferences load");
        if (!_prefs.load(result)) {
            s_log->error("unable to load preferences");
            s_log->result(result);
            return false;
        }

        s_log->info("hardware initialize");
        if (!hardware::initialize(result, _executable_path)) {
            s_log->error("hardware initialization failed:");
            s_log->result(result);
            return false;
        }

        s_log->info("engine initialize");
        if (!_engine.initialize(result, &_prefs)) {
            s_log->error("engine initialization failed:");
            s_log->result(result);
            return false;
        }

        s_log->info("ide configuration");
        if (!configure_ide(result)) {
            s_log->error("ide configuration failed:");
            s_log->result(result);
            return false;
        }

        s_log->info("emulator configuration");
        if (!configure_emulator(result)) {
            s_log->error("emulator initialization failed:");
            s_log->result(result);
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

        s_log->info("shutdown start");

        _prefs.default_path(boost::filesystem::current_path());
        s_log->info(fmt::format(
            "preferences default path: {}",
            _prefs.default_path().string()));

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

        s_log->info("save preferences");
        if (!_prefs.save(result)) {
            s_log->error("preferences save failed:");
            s_log->result(result);
            return false;
        }

        s_log->info("release font cache resources");
        core::font_book::instance()->shutdown();

        s_log->info("engine shutdown");
        if (!_engine.shutdown(result, _prefs)) {
            s_log->error("engine shutdown failed:");
            s_log->result(result);
            return false;
        }

        return true;
    }

    int application::run(int argc, char** argv) {
        if (!init(argc, argv))
            return 1;

        _engine.focus(_ide_context.id());

        core::result result;

        s_log->info("engine run");
        if (!_engine.run(result)) {
            s_log->error("engine run failed:");
            s_log->result(result);
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
        return _ide_context.initialize(result, _engine.clip_rect());
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
        return _emulator_context.initialize(result, _engine.clip_rect());
    }

}