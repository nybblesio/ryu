//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include "cpu.h"

namespace ryu::hardware {

    cpu::cpu(int id, const std::string& name) : integrated_circuit(id, name) {
    }

}