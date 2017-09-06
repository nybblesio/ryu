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

#include <string>
#include <utility>
#include <hardware/machine.h>
#include <boost/filesystem/path.hpp>
#include "result.h"

namespace ryu::core {

    namespace fs = boost::filesystem;

    class project {
    public:
        explicit project(std::string name);

        std::string name() const;

        hardware::machine* machine();

        bool save(core::result& result);

        void machine(hardware::machine* machine);

        bool load(core::result& result, const fs::path& file_name);

    private:
        std::string _name;
        fs::path _file_name;
        hardware::machine* _machine = nullptr;
    };

};

