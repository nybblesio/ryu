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
        if (_ic != nullptr)
            _ic->component(this);
    }

    component::~component() {
        delete _ic;
        _ic = nullptr;
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

    hardware::machine* component::machine() {
        return _machine;
    }

    std::string component::description() const {
        return _description;
    }

    void component::name(const std::string& value) {
        _name = value;
    }

    ryu::core::palette_index component::color() const {
        return _color;
    }

    void component::machine(hardware::machine* value) {
        _machine = value;
    }

    hardware::integrated_circuit* component::ic() const {
        return _ic;
    }

    void component::description(const std::string& value) {
        _description = value;
    }

    void component::color(ryu::core::palette_index value) {
        _color = value;
    }

    void component::ic(hardware::integrated_circuit* value) {
        if (_ic != value) {
            _ic = value;
            if (_ic != nullptr)
                _ic->component(this);
        }
    }

}