//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "project.h"

namespace ryu::core {

    project::project(const std::string& name) : _name(name) {
    }

    std::string project::name() const {
        return _name;
    }

    hardware::machine* project::machine() {
        return _machine;
    }

    bool project::save(core::result& result) {
        return false;
    }

    void project::machine(hardware::machine* machine) {
        _machine = machine;
    }

    bool project::load(core::result& result, const fs::path& file_name) {
        _file_name = file_name;
        return false;
    }

}