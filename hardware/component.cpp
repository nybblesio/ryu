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
            uint32_t id,
            hardware::integrated_circuit* ic) : _id(id),
                                                _ic(ic) {
    }

    uint32_t component::id() const {
        return _id;
    }

    std::string component::name() const {
        return _name;
    }

    uint32_t component::address() const {
        return _address;
    }

    void component::address(uint32_t value) {
        _address = value;
    }

    std::string component::description() const {
        return _description;
    }

    void component::name(const std::string& value) {
        _name = value;
    }

    hardware::integrated_circuit* component::ic() const {
        return _ic;
    }

    void component::description(const std::string& value) {
        _description = value;
    }

}