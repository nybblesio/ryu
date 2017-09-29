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

namespace ryu::ide {

    class command_factory {
    public:
        command_factory() = default;

        ~command_factory() = default;

        core::symbol_table* symbol_table();

        bool execute(core::result& result, const std::string& line);

        bool load(core::result& result, const boost::filesystem::path& path);

        bool save(core::result& result, const boost::filesystem::path& path);

    private:
        core::symbol_table _symbol_table;
    };

};

