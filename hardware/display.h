//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <string>
#include "hardware_types.h"

namespace ryu::hardware {

    class display {
    public:
        static display_list& catalog();

        display(
                uint32_t id,
                const std::string& name,
                uint16_t width,
                uint16_t height,
                uint8_t refresh_rate = 60);

        uint32_t id() const;

        uint16_t width() const;

        uint16_t height() const;

        std::string name() const;

        uint8_t refresh_rate() const;

    private:
        static display_list _displays;

        uint32_t _id {};
        std::string _name;
        uint16_t _width = 0;
        uint16_t _height = 0;
        uint8_t _refresh_rate = 60;
    };

};
