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

    namespace fs = boost::filesystem;

    class application {
    public:
        application() = default;

        bool shutdown();

        int run(int argc, char** argv);

        bool init(int argc, char** argv);

    private:
        bool configure_ide(core::result& result);

        bool configure_emulator(core::result& result);

        void show_result_messages(core::result& result);

    private:
        core::engine _engine {};
        fs::path _executable_path;
        core::preferences _prefs {};
        ide::ide_context _ide_context {"ide"};
        emulator::emulator_context _emulator_context {"emulator"};
    };

};

