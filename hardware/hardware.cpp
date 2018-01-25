//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "hardware.h"
#include "registry.h"

namespace ryu::hardware {

    bool initialize(
            core::result& result,
            const fs::path& executable_path) {
        ram::init();
        rom::init();
        memory_mapper::init();
        mc6809::cpu::init();

        // XXX: eventually, add code to dynamically load .so/.dll

        fs::path machine_registry_path(executable_path);
        machine_registry_path.append("machines.yaml");

        return hardware::registry::instance()->load(
                result,
                machine_registry_path);
    }

}