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

#include "core_types.h"

namespace ryu::core {

    class hex_formatter {
    public:
        static std::string dump_to_string(
            const void* data,
            size_t size);

        static core::data_table_t dump_to_table(
            const void* data,
            size_t size,
            uint8_t bytes = 8);
    };

};

