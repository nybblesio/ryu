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

#include <vector>
#include "palette_entry.h"

namespace ryu::core {

    class palette {
    public:
        palette();

        explicit palette(size_t size);

        virtual ~palette();

        inline size_t size() const {
            return _entries.size();
        }

        palette_entry& get(size_t index);

        palette_entry& operator[] (size_t index);

    private:
        void add_missing_entries(size_t size);

    private:
        std::vector<palette_entry> _entries {};
    };

};