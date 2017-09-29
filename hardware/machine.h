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
#include "memory_mapper.h"
#include "hardware_types.h"

namespace ryu::hardware {

    class machine {
    public:
        explicit machine(int id);

        virtual ~machine() = default;

        int id() const;

        void initialize();

        std::string name() const;

        void remove_component(int id);

        uint32_t address_space() const;

        hardware::memory_mapper* mapper();

        hardware::display* display() const;

        void address_space(uint32_t value);

        void name(const std::string& value);

        const component_list components() const;

        void display(hardware::display* display);

        hardware::component* find_component(int id) const;

        void add_component(hardware::component* component, uint32_t address);

    protected:
        virtual void on_initialize();

    private:
        int _id = -1;
        std::string _name;
        component_dict _components;
        hardware::memory_mapper _mapper;
        uint32_t _address_space = 2048 * 1024;
        hardware::display* _display = nullptr;
    };

};
