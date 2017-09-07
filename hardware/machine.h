//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <string>
#include "display.h"
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

        hardware::display* display() const;

        hardware::memory_map* memory_map() const;

        void display(hardware::display* display);

        void memory_map(hardware::memory_map* map);

    protected:
        virtual void on_initialize();

    private:
        int _id = -1;
        std::string _name;
        hardware::display* _display = nullptr;
        hardware::memory_map* _memory_map = nullptr;
    };

};
