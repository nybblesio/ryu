//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "palette.h"

namespace ryu::core {

    palette::palette() {
    }

    palette::palette(size_t size) {
        add_missing_entries(size);
    }

    palette::~palette() {
    }

    palette_entry& palette::get(size_t index) {
        add_missing_entries(index + 1);
        return _entries[index];
    }

    void palette::add_missing_entries(size_t size) {
        if (size < _entries.size())
            return;
        auto missing_entry_count = size - _entries.size();
        for (size_t i = 0; i < missing_entry_count; i++)
            _entries.emplace_back(0, 0, 0, 0);
    }

    palette_entry& palette::operator[](size_t index) {
        return get(index);
    }

}