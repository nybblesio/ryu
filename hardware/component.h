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
#include <cstdint>
#include "memory_mapper.h"
#include "integrated_circuit.h"

namespace ryu::hardware {

    class component {
    public:
        component(
            uint32_t id,
            hardware::integrated_circuit* ic);

        virtual ~component();

        uint32_t id() const;

        uint32_t address() const;

        std::string name() const;

        void address(uint32_t value);

        hardware::machine* machine();

        std::string description() const;

        void name(const std::string& value);

        void machine(hardware::machine* value);

        ryu::core::palette_index color() const;

        hardware::integrated_circuit* ic() const;

        void color(ryu::core::palette_index value);

        void description(const std::string& value);

        void ic(hardware::integrated_circuit* value);

    private:
        uint32_t _id;
        uint32_t _address;
        std::string _name;
        std::string _description;
        ryu::core::palette_index _color;
        hardware::machine* _machine = nullptr;
        hardware::integrated_circuit* _ic = nullptr;
    };

};