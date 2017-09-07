//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include "integrated_circuit.h"

namespace ryu::hardware {

    integrated_circuit::integrated_circuit(int id, const std::string& name) : _id(id),
                                                                              _name(name) {
    }

    void integrated_circuit::zero() {
    }

    int integrated_circuit::id() const {
        return _id;
    }

    void integrated_circuit::fill(uint8_t value) {
    }

    uint32_t integrated_circuit::address() const {
        return _address;
    }

    std::string integrated_circuit::name() const {
        return _name;
    }

    uint32_t integrated_circuit::last_address() const {
        return _address + address_space();
    }

    uint32_t integrated_circuit::address_space() const {
        return 0;
    }

    void integrated_circuit::address(uint32_t address) {
        _address = address;
    }

    uint8_t integrated_circuit::read_byte(uint32_t address) const {
        return 0;
    }

    void integrated_circuit::write_byte(uint32_t address, uint8_t value) {

    }

}