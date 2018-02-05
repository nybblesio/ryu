//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include <core/id_pool.h>
#include "machine.h"

namespace ryu::hardware {

    machine::machine(uint32_t id) : _id(id),
                                    _mapper() {
    }

    uint32_t machine::id() const {
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

    uint32_t machine::address_space() const {
        return _address_space;
    }

    std::string machine::description() const {
        return _description;
    }

    void machine::set_write_latches(bool flag) {
        for (auto it = _components.begin();
             it != _components.end();
             ++it) {
            auto ic = it->second->ic();
            if (ic != nullptr)
                ic->write_latch(flag);
        }
    }

    void machine::remove_component(uint32_t id) {
        auto component = find_component(id);
        if (component != nullptr) {
            _mapper.release(component->ic());
            _components.erase(id);
        }
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

    void machine::description(const std::string& value) {
        _description = value;
    }

    void machine::add_component(hardware::component* component) {
        if (component == nullptr)
            return;
        _components.insert(std::make_pair(component->id(), component));
        _mapper.reserve(component->address(), component->ic());
    }

    hardware::component* machine::find_component(uint32_t id) const {
        auto it = _components.find(id);
        if (it != _components.end())
            return it->second;
        return nullptr;
    }

    hardware::component* machine::find_component(const std::string& name) {
        for (auto it = _components.begin();
             it != _components.end();
             ++it) {
            if (it->second->name() == name)
                return it->second;
        }
        return nullptr;
    }

}