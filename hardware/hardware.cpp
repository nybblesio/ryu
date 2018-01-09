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

        rttr::type::get<integrated_circuit>();
        rttr::type::get<memory_mapper>();
        rttr::type::get<ram>();
        rttr::type::get<rom>();
        rttr::type::get<mc6809::cpu>();
        mc6809::cpu::init();

        return true;
    }

}