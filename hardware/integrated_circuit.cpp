//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include <rttr/registration>
#include "integrated_circuit.h"

RTTR_REGISTRATION {
    rttr::registration::class_<ryu::hardware::integrated_circuit>("ryu::hardware::integrated_circuit")
            .constructor<const std::string&>()
            .property_readonly("id", &ryu::hardware::integrated_circuit::id)
            .property_readonly("type", &ryu::hardware::integrated_circuit::type)
            .property_readonly("name", &ryu::hardware::integrated_circuit::name)
            .property_readonly("address_space", &ryu::hardware::integrated_circuit::address_space);
}

namespace ryu::hardware {

    integrated_circuit::integrated_circuit(
            const std::string& name) : _id(core::id_pool::instance()->allocate_from_range(256, 32767)),
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

    std::string integrated_circuit::type() const {
        return "integrated_circuit";
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

    const hardware::memory_map& integrated_circuit::memory_map() const {
        return _memory_map;
    }

    void integrated_circuit::write_byte(uint32_t address, uint8_t value) {
    }

}