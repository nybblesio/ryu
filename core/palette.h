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

#include "palette_entry.h"

namespace ryu::core {

    class palette {
    public:
        const static int max_entries = 256;

        palette() = default;

        palette_entry& get(size_t index) {
            if (index > max_entries)
                return _entries[max_entries - 1];
            return _entries[index];
        }

        palette_entry& operator[] (size_t index) {
            return get(index);
        }

    private:
        palette_entry _entries[max_entries] {};
    };

};

