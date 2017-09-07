//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include "machine.h"

namespace ryu::hardware {

    machine::machine(int id, const std::string& name) : _id(id),
                                                        _name(name) {
    }

    int machine::id() const {
        return _id;
    }

    void machine::initialize() {
        on_initialize();
    }

    void machine::on_initialize() {
    }

    std::string machine::name() const {
        return _name;
    }

    hardware::display* machine::display() const {
        return _display;
    }

    void machine::display(hardware::display* display) {
        _display = display;
    }

    hardware::memory_map* machine::memory_map() const {
        return _memory_map;
    }

    void machine::memory_map(hardware::memory_map* map) {
        _memory_map = map;
    }

}