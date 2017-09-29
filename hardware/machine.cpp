//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include <core/id_pool.h>
#include "machine.h"

namespace ryu::hardware {

    machine::machine(int id) : _id(id),
                               _mapper(core::id_pool::instance()->allocate(), "mapper") {
    }

    int machine::id() const {
        return _id;
    }

    void machine::initialize() {
        on_initialize();
    }

    void machine::on_initialize() {
    }

    std::string machine::name() const {
        return _name;
    }

    void machine::remove_component(int id) {
        auto component = find_component(id);
        if (component != nullptr) {
            _mapper.release(component->ic());
            _components.erase(id);
        }
    }

    uint32_t machine::address_space() const {
        return _address_space;
    }

    hardware::memory_mapper* machine::mapper() {
        return &_mapper;
    }

    void machine::address_space(uint32_t value) {
        _address_space = value;
        _mapper.address_space(value);
    }

    hardware::display* machine::display() const {
        return _display;
    }

    void machine::name(const std::string& value) {
        _name = value;
    }

    const component_list machine::components() const {
        component_list list;
        for (const auto& it : _components)
            list.push_back(it.second);
        return list;
    }

    void machine::display(hardware::display* display) {
        _display = display;
    }

    hardware::component* machine::find_component(int id) const {
        auto it = _components.find(id);
        if (it != _components.end())
            return it->second;
        return nullptr;
    }

    void machine::add_component(hardware::component* component, uint32_t address) {
        if (component == nullptr)
            return;
        _components.insert(std::make_pair(component->id(), component));
        _mapper.reserve(address, component->ic());
    }

}