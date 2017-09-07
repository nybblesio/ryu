//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "component.h"

namespace ryu::hardware {

    component::component(
            int id,
            const std::string& name,
            hardware::integrated_circuit* ic) : _id(id),
                                                _name(name),
                                                _ic(ic) {
    }

    int component::id() const {
        return _id;
    }

    std::string component::name() const {
        return _name;
    }

    hardware::integrated_circuit* component::ic() const {
        return _ic;
    }

    hardware::memory_map* component::memory_map() const {
        return _memory_map;
    }

    void component::memory_map(hardware::memory_map* map) {
        _memory_map = map;
    }

}