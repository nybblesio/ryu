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

    access_type_flags memory_mapper::access_type() const {
        return access_types::writable | access_types::readable;
    }

    uint8_t memory_mapper::read_byte(uint32_t address) const {
        auto circuit = circuit_at_address(access_types::readable, address);
        if (circuit.ic != nullptr)
            return circuit.ic->read_byte(address - circuit.start);
        return 0;
    }

    void memory_mapper::write_byte(uint32_t address, uint8_t value) {
        auto circuit = circuit_at_address(access_types::writable, address);
        if (circuit.ic != nullptr)
            circuit.ic->write_byte(address - circuit.start, value);
    }

    void memory_mapper::release(hardware::integrated_circuit* component) {
        for (auto it = _components.begin(); it != _components.end(); ++it) {
            if ((*it).value == component) {
                it = _components.erase(it);
            }
        }
    }

    // XXX: this implementation is not correct, spend time with it
    memory_mapper::component_address_space_t memory_mapper::circuit_at_address(
            integrated_circuit::access_types access_type,
            uint32_t address) const {
        IntervalTree<hardware::integrated_circuit*> tree(const_cast<ic_interval_list&>(_components));
        ic_interval_list results;
        tree.findOverlapping(address, address, results);
        if (results.empty()) {
            return {};
        }
        for (auto range : results) {
            auto ic_access_type = range.value->access_type();
            if (ic_access_type == access_types::none)
                continue;
            if (((ic_access_type & readable) != 0 && access_type == readable)
            ||  ((ic_access_type & writable) != 0 && access_type == writable)) {
                return {
                    static_cast<uint32_t>(range.start),
                    static_cast<uint32_t>(range.stop),
                    range.value
                };
            }
        }
        return {};
    }

}