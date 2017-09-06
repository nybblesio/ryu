//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include "display.h"

namespace ryu::hardware {

    display::display(
            int id,
            const std::string& name,
            int width,
            int height,
            int refresh_rate) : _id(id),
                                _name(name),
                                _width(width),
                                _height(height),
                                _refresh_rate(refresh_rate) {
    }

    int display::id() const {
        return _id;
    }

    int display::width() const {
        return _width;
    }

    int display::height() const {
        return _height;
    }

    int display::refresh_rate() const {
        return _refresh_rate;
    }

    std::string display::name() const {
        return _name;
    }

}