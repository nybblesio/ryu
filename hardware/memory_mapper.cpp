//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "memory_mapper.h"

RTTR_REGISTRATION {
    rttr::registration::class_<ryu::hardware::memory_mapper>("ryu::hardware::memory_mapper") (
        rttr::metadata(ryu::hardware::meta_data_key::type_id, ryu::hardware::memory_mapper_id),
        rttr::metadata(ryu::hardware::meta_data_key::type_name, "Memory Mapper IC")
    )
    .constructor<>(rttr::registration::public_access) (
            rttr::policy::ctor::as_raw_ptr
    );
}

namespace ryu::hardware {

    void memory_mapper::init() {
    }

    memory_mapper::memory_mapper() : integrated_circuit("memory-mapper-ic") {
    }

    void memory_mapper::zero() {
        for (auto& ic : _components)
            ic.value->zero();
    }

    void memory_mapper::reserve(
            uint32_t address,
            hardware::integrated_circuit* component) {
        if (component == nullptr)
            return;
        _components.emplace_back(
                address,
                address + component->address_space(),
                component);
    }

    void memory_mapper::clear() {
        _components.clear();
    }

    void memory_mapper::fill(uint8_t value) {
        for (auto& ic : _components)
            ic.value->fill(value);
    }

    uint8_t memory_mapper::read_byte(uint32_t address) const {
        auto ic = circuit_at_address(address);
        if (ic != nullptr)
            return ic->read_byte(address);
        return 0;
    }

    void memory_mapper::write_byte(uint32_t address, uint8_t value) {
        auto ic = circuit_at_address(address);
        if (ic != nullptr)
            ic->write_byte(address, value);
    }

    void memory_mapper::release(hardware::integrated_circuit* component) {
        for (auto it = _components.begin(); it != _components.end(); ++it) {
            if ((*it).value == component) {
                it = _components.erase(it);
            }
        }
    }

    integrated_circuit* memory_mapper::circuit_at_address(uint32_t address) const {
        IntervalTree<hardware::integrated_circuit*> tree(const_cast<ic_interval_list&>(_components));
        ic_interval_list results;
        tree.findContained(address, address, results);
        if (results.empty())
            return nullptr;
        return results[0].value;
    }

}