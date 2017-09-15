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
    rttr::registration::class_<ryu::hardware::memory_mapper>("ryu::hardware::memory_mapper");
}

namespace ryu::hardware {

    memory_mapper::memory_mapper(
            int id,
            const std::string& name,
            uint32_t address_space) : integrated_circuit(id, name),
                                      _address_space(address_space) {
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
        component->address(address);
        _components.emplace_back(address, component->last_address(), component);
    }

    void memory_mapper::clear() {
        _components.clear();
    }

    void memory_mapper::fill(uint8_t value) {
        for (auto& ic : _components)
            ic.value->fill(value);
    }

    uint32_t memory_mapper::address_space() const {
        return _address_space;
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

    integrated_circuit* memory_mapper::circuit_at_address(uint32_t address) const {
        IntervalTree<hardware::integrated_circuit*> tree(const_cast<ic_interval_list&>(_components));
        ic_interval_list results;
        tree.findContained(address, address, results);
        if (results.empty())
            return nullptr;
        return results[0].value;
    }

}