//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include "machine.h"

namespace ryu::hardware {

    machine::machine(int id, const std::string& name) : _id(id),
                                                        _name(name) {
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
        _components.erase(id);
    }

    hardware::display* machine::display() const {
        return _display;
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

    void machine::add_component(hardware::component* component) {
        if (component == nullptr)
            return;
        _components.insert(std::make_pair(component->id(), component));
    }

}