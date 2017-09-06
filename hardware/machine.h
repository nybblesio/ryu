//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <string>
#include "display.h"
#include "hardware_types.h"

namespace ryu::hardware {

    class machine {
    public:
        machine(int id, const std::string& name);

        virtual ~machine() = default;

        void init();

        int id() const;

        std::string name() const;

        hardware::display* display() const;

        void display(hardware::display* display);

    protected:
        virtual void on_init() = 0;

    private:
        int _id = -1;
        std::string _name;
        hardware::display* _display = nullptr;
    };

};
