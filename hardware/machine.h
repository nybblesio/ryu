//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <string>
#include "display.h"
#include "component.h"
#include "memory_map.h"
#include "hardware_types.h"

namespace ryu::hardware {

    class machine {
    public:
        machine(int id, const std::string& name);

        virtual ~machine() = default;

        int id() const;

        void initialize();

        std::string name() const;

        void remove_component(int id);

        hardware::display* display() const;

        const component_list components() const;

        void display(hardware::display* display);

        hardware::component* find_component(int id) const;

        void add_component(hardware::component* component);

    protected:
        virtual void on_initialize();

    private:
        int _id = -1;
        std::string _name;
        component_dict _components;
        hardware::display* _display = nullptr;
    };

};
