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
                int id,
                const std::string& name,
                int width,
                int height,
                int refresh_rate = 60);

        int id() const;

        int width() const;

        int height() const;

        int refresh_rate() const;

        std::string name() const;

    private:
        static display_list _displays;

        int _id = -1;
        int _width = 0;
        int _height = 0;
        std::string _name;
        int _refresh_rate = 60;
    };

};
