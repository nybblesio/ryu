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
        palette() = default;

        inline size_t size() const {
            return _entries.size();
        }

        palette_entry& get(size_t index) {
            add_missing_entries(index + 1);
            return _entries[index];
        }

        palette_entry& operator[] (size_t index) {
            return get(index);
        }

    private:
        void add_missing_entries(size_t size) {
            if (size < _entries.size())
                return;
            auto missing_entry_count = size - _entries.size();
            for (auto i = 0; i < missing_entry_count; i++)
                _entries.emplace_back(0, 0, 0, 0);
        }

    private:
        std::vector<palette_entry> _entries {};
    };

};

