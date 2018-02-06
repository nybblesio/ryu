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
#include <boost/filesystem/path.hpp>
#include "ram.h"
#include "rom.h"
#include "dip_switch.h"
#include "memory_mapper.h"
#include "hardware_types.h"
#include "cpus/mc6809/cpu.h"

namespace ryu::hardware {

    namespace fs = boost::filesystem;

    bool initialize(
            core::result& result,
            const fs::path& executable_path);

}