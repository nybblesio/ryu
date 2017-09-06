//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//


#pragma once

#include <string>
#include "registry.h"
#include "types.h"

namespace ryu::cpu {

    class model {
    public:
        model() = default;

        model(
                models::ids id,
                const std::string& name,
                const std::string& description,
                const std::string& manufacturer,
                int address_space);

        models::ids id() const;

        std::string name() const;

        int address_space() const;

        std::string description() const;

        std::string manufacturer() const;

    private:
        models::ids _id;
        std::string _name;
        int _address_space;
        std::string _description;
        std::string _manufacturer;
    };

};

