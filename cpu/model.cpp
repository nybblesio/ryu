//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "model.h"

namespace ryu::cpu {

    model::model(
            models::ids id,
            const std::string& name,
            const std::string& description,
            const std::string& manufacturer,
            int address_space) : _id(id),
                                 _name(name),
                                 _address_space(address_space),
                                 _description(description),
                                 _manufacturer(manufacturer) {
    }

    std::string model::name() const {
        return _name;
    }

    int model::address_space() const {
        return _address_space;
    }

    std::string model::description() const {
        return _description;
    }

    std::string model::manufacturer() const {
        return _manufacturer;
    }

    models::ids model::id() const {
        return _id;
    }

}