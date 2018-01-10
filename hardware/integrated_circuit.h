//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <string>
#include <rttr/registration>
#include "memory_map.h"
#include "hardware_types.h"

namespace ryu::hardware {

    class integrated_circuit {
    public:
        explicit integrated_circuit(const std::string& name);

        virtual ~integrated_circuit() = default;

        uint32_t id() const;

        virtual void zero();

        std::string name() const;

        uint32_t address_space() const;

        virtual void fill(uint8_t value);

        void address_space(uint32_t value);

        const hardware::memory_map& memory_map() const;

        virtual uint8_t read_byte(uint32_t address) const;

        virtual void write_byte(uint32_t address, uint8_t value);

        RTTR_ENABLE()

    protected:
        virtual void on_address_space_changed();

    private:
        uint32_t _id {};
        std::string _name;
        uint32_t _address_space;
        hardware::memory_map _memory_map;
    };

};
