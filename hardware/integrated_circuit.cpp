//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include "integrated_circuit.h"

namespace ryu::hardware {

    integrated_circuit::integrated_circuit(int id, const std::string& name) : _id(id),
                                                                              _name(name) {
    }

    int integrated_circuit::id() const {
        return _id;
    }

    std::string integrated_circuit::name() const {
        return _name;
    }

}