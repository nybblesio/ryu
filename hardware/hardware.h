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
#include "hardware_types.h"
#include "integrated_circuit.h"
#include "memory_mapper.h"
#include "ram.h"
#include "rom.h"
#include "cpus/mc6809/cpu.h"

namespace ryu::hardware {

    bool initialize(core::result& result);

}