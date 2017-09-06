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

    void machine::init() {
        on_init();
    }

    int machine::id() const {
        return _id;
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

}