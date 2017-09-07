//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <string>

namespace ryu::hardware {

    class memory_map;

    class integrated_circuit {
    public:
        integrated_circuit(int id, const std::string& name);

        int id() const;

        virtual void zero();

        uint32_t address() const;

        std::string name() const;

        uint32_t last_address() const;

        void address(uint32_t address);

        virtual void fill(uint8_t value);

        virtual uint32_t address_space() const;

        virtual uint8_t read_byte(uint32_t address) const;

        virtual void write_byte(uint32_t address, uint8_t value);

    private:
        int _id = -1;
        std::string _name;
        uint32_t _address = 0;
    };

};
