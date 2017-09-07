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
#include "integrated_circuit.h"
#include "memory_map.h"

namespace ryu::hardware {

    class component {
    public:
        component(
                int id,
                const std::string& name,
                hardware::integrated_circuit* ic);

        int id() const;

        std::string name() const;

        hardware::memory_map* memory_map() const;

        hardware::integrated_circuit* ic() const;

        void memory_map(hardware::memory_map* map);

    private:
        int _id;
        std::string _name;
        hardware::memory_map* _memory_map = nullptr;
        hardware::integrated_circuit* _ic = nullptr;
    };

};

