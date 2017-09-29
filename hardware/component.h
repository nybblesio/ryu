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
#include "memory_mapper.h"
#include "integrated_circuit.h"

namespace ryu::hardware {

    class component {
    public:
        component(
                int id,
                hardware::integrated_circuit* ic);

        int id() const;

        std::string name() const;

        void name(const std::string& value);

        hardware::integrated_circuit* ic() const;

    private:
        int _id;
        std::string _name;
        hardware::integrated_circuit* _ic = nullptr;
    };

};

