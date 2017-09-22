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

namespace ryu::ide {

    class command_factory {
    public:
        command_factory() = default;

        ~command_factory() = default;

        bool execute(core::result& result, const std::string& line);
    };

};

