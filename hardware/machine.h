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
        explicit machine(uint32_t id);

        virtual ~machine() = default;

        void initialize();

        uint32_t id() const;

        std::string name() const;

        uint32_t address_space() const;

        std::string description() const;

        hardware::memory_mapper* mapper();

        void set_write_latches(bool flag);

        void remove_component(uint32_t id);

        hardware::display* display() const;

        void address_space(uint32_t value);

        void name(const std::string& value);

        const component_list components() const;

        void display(hardware::display* display);

        void description(const std::string& value);

        void add_component(hardware::component* component);

        hardware::component* find_component(uint32_t id) const;

        hardware::component* find_component(const std::string& name);

    protected:
        virtual void on_initialize();

    private:
        uint32_t _id {};
        std::string _name;
        std::string _description;
        component_dict _components;
        hardware::memory_mapper _mapper;
        uint32_t _address_space = 2048 * 1024;
        hardware::display* _display = nullptr;
    };

};
