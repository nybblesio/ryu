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
#include <boost/filesystem.hpp>
#include <hardware/hardware_types.h>

namespace ryu::machine {

    namespace fs = boost::filesystem;

    class registry {
    public:
        registry() = default;

        bool load(core::result& result, const fs::path& path);

    private:
    };

};

