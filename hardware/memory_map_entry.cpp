//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "memory_map_entry.h"

namespace ryu::hardware {
    memory_map_entry::memory_map_entry(
            uint32_t offset,
            uint32_t size,
            const std::string& name,
            const std::string& description,
            const read_callable& reader,
            const write_callable& writer) : _size(size),
                                            _offset(offset),
                                            _name(name),
                                            _description(description),
                                            _read_callable(reader),
                                            _write_callable(writer) {
    }

    uint32_t memory_map_entry::size() const {
        return _size;
    }

    uint32_t memory_map_entry::offset() const {
        return _offset;
    }

    std::string memory_map_entry::name() const {
        return _name;
    }

    std::string memory_map_entry::description() const {
        return _description;
    }

    uint8_t memory_map_entry::invoke_reader(uint32_t offset) const {
        if (_read_callable == nullptr)
            return 0;
        return _read_callable(offset);
    }

    void memory_map_entry::invoke_writer(uint32_t offset, uint8_t value) {
        if (_write_callable == nullptr)
            return;
        _write_callable(offset, value);
    }
}