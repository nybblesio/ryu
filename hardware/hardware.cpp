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

namespace ryu::hardware {

    bool initialize(core::result& result) {
        ram::init();
        rom::init();
        memory_mapper::init();
        mc6809::cpu::init();
        return true;
    }

}