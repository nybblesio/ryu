//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//


#pragma once

#include <core/engine.h>
#include <ide/ide_context.h>
#include <core/preferences.h>
#include <emulator/emulator_context.h>

namespace ryu {

    class application {
    public:
        static const int display_width = 1920;
        static const int display_height = 1080;

        application();

        bool shutdown();

        int run(int argc, char** argv);

        bool init(int argc, char** argv);

    private:
        bool configure_ide(core::result& result);

        bool configure_emulator(core::result& result);

        core::rect ide_bounds(const core::rect& bounds);

        void show_result_messages(core::result& result);

        core::rect emulator_bounds(const core::rect& bounds);

    private:
        core::engine _engine;
        core::preferences _prefs;
        ide::ide_context _ide_context;
        emulator::emulator_context _emulator_context;
    };

};

