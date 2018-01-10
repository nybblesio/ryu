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

#include <core/result.h>
#include <core/symbol_table.h>
#include "project.h"

namespace ryu::core {

    class environment {
    public:
        environment() = default;

        ~environment() = default;

        core::symbol_table* symbol_table();

        bool assemble(core::result& result, core::project& project);

        bool execute(core::result& result, const std::string& line);

        // N.B. loads a .s file that has environment config
        bool load(core::result& result, const boost::filesystem::path& path);

        // N.B. saves a .s file that has environment config
        bool save(core::result& result, const boost::filesystem::path& path);

    private:
        core::symbol_table _symbol_table;
    };

};

