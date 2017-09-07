//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "memory_map.h"

namespace ryu::hardware {

    memory_map::memory_map(
            int id,
            const std::string& name,
            uint32_t address_space) : integrated_circuit(id, name),
                                      _address_space(address_space) {
    }

    void memory_map::zero() {
        for (auto& ic : _components)
            ic.value->zero();
    }

    void memory_map::reserve(
            uint32_t address,
            hardware::integrated_circuit* component) {
        if (component == nullptr)
            return;
        component->address(address);
        _components.push_back(Interval<hardware::integrated_circuit*>(
                address,
                component->last_address(),
                component));
    }

    void memory_map::clear() {
        _components.clear();
    }

    void memory_map::fill(uint8_t value) {
        for (auto& ic : _components)
            ic.value->fill(value);
    }

    uint32_t memory_map::address_space() const {
        return _address_space;
    }

    uint8_t memory_map::read_byte(uint32_t address) const {
        auto results = components_at_address(address);
        if (results.empty())
            return 0;
        return results[0].value->read_byte(address);
    }

    void memory_map::write_byte(uint32_t address, uint8_t value) {
        auto results = components_at_address(address);
        if (results.empty())
            return;
        return results[0].value->write_byte(address, value);
    }

    ic_interval_list memory_map::components_at_address(uint32_t address) const {
        IntervalTree<hardware::integrated_circuit*> tree(const_cast<ic_interval_list&>(_components));
        ic_interval_list results;
        tree.findContained(address, address, results);
        return results;
    }

}