//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include "integrated_circuit.h"

namespace ryu::hardware {

    class cpu : public integrated_circuit {
    public:
        cpu(int id, const std::string& name);

    private:
    };

};
