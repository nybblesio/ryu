//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <string>

namespace ryu::hardware {

    class integrated_circuit {
    public:
        integrated_circuit(int id, const std::string& name);

        int id() const;

        std::string name() const;

    private:
        int _id = -1;
        std::string _name;
    };

};
