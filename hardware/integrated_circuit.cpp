//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include <rttr/registration>
#include "integrated_circuit.h"

RTTR_REGISTRATION {
    rttr::registration::class_<ryu::hardware::integrated_circuit>("ryu::hardware::integrated_circuit") (
        rttr::metadata(ryu::hardware::meta_data_key::type_id, ryu::hardware::integrated_circuit_id),
        rttr::metadata(ryu::hardware::meta_data_key::type_name, "Base IC")
    )
    .constructor<const std::string&>(rttr::registration::public_access) (
            rttr::policy::ctor::as_raw_ptr
    )
    .property_readonly("id", &ryu::hardware::integrated_circuit::id)
    .property_readonly("name", &ryu::hardware::integrated_circuit::name);
}

namespace ryu::hardware {

    integrated_circuit::integrated_circuit(const std::string& name) : _id(core::id_pool::instance()->allocate()),
                                                                      _name(name) {
    }

    void integrated_circuit::zero() {
    }

    uint32_t integrated_circuit::id() const {
        return _id;
    }

    void integrated_circuit::fill(uint8_t value) {
    }

    std::string integrated_circuit::name() const {
        return _name;
    }

    uint32_t integrated_circuit::address_space() const {
        return _address_space;
    }

    void integrated_circuit::on_address_space_changed() {
    }

    void integrated_circuit::address_space(uint32_t value) {
        if (value != _address_space) {
            _address_space = value;
            on_address_space_changed();
        }
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