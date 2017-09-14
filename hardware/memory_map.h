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

#include "hardware_types.h"
#include "memory_map_entry.h"

namespace ryu::hardware {

    class memory_map {
    public:
        memory_map() = default;

        virtual ~memory_map();

        void add(
                uint32_t offset,
                uint32_t size,
                const std::string& name,
                const std::string& description,
                const memory_map_entry::read_callable& reader,
                const memory_map_entry::write_callable& writer);

        memory_map_entry* entry_at_offset(uint32_t offset) const;

    private:
        mm_entry_interval_list _entries {};
    };

};

