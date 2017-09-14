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
            hardware::integrated_circuit* ic) : _id(id),
                                                _ic(ic) {
    }

    int component::id() const {
        return _id;
    }

    std::string component::name() const {
        return _name;
    }

    void component::name(const std::string& value) {
        _name = value;
    }

    hardware::memory_mapper* component::mapper() const {
        return _mapper;
    }

    hardware::integrated_circuit* component::ic() const {
        return _ic;
    }

    void component::mapper(hardware::memory_mapper* mapper) {
        _mapper = mapper;
    }

}