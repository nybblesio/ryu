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

namespace ryu::hardware {

    namespace fs = boost::filesystem;

    class registry {
    public:
        static registry* instance();

        machine_list machines();

        void remove_machine(int id);

        hardware::machine* new_machine();

        hardware::machine* find_machine(int id);

        bool save(core::result& result, const fs::path& path);

        bool load(core::result& result, const fs::path& path);

    protected:
        registry() = default;

        virtual ~registry() = default;

    private:
        machine_dict _machines;
    };

};

