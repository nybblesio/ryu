//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "memory_map.h"

namespace ryu::hardware {

    memory_map::~memory_map() {
        for (const auto& it : _entries)
            delete it.value;
    }

    void memory_map::clear() {
        _entries.clear();
    }

    void memory_map::add(
            uint32_t offset,
            uint32_t size,
            const std::string& name,
            const std::string& description,
            memory_map_entry::memory_map_entry_flags flags) {
        auto entry = new memory_map_entry(offset, size, name, description, flags);
        _entries.emplace_back(offset, offset + size, entry);
    }

    void memory_map::add(
            uint32_t offset,
            uint32_t size,
            const std::string& name,
            const std::string& description,
            const memory_map_entry::read_callable& reader,
            const memory_map_entry::write_callable& writer,
            memory_map_entry::memory_map_entry_flags flags) {
        auto entry = new memory_map_entry(offset, size, name, description, reader, writer, flags);
        _entries.emplace_back(offset, offset + size, entry);
    }

    const mm_entry_interval_list& memory_map::entries() const {
        return _entries;
    }

    memory_map_entry* memory_map::entry_at_offset(uint32_t offset) const {
        IntervalTree<hardware::memory_map_entry*> tree(const_cast<mm_entry_interval_list&>(_entries));
        mm_entry_interval_list results;
        tree.findContained(offset, offset, results);
        if (results.empty())
            return nullptr;
        return results[0].value;
    }

}