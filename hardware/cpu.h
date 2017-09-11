//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include "integrated_circuit.h"

namespace ryu::hardware {

    class cpu : public hardware::integrated_circuit {
    public:
        cpu(int id, const std::string& name);

        std::string type() const override {
            return "cpu";
        }

    private:
        RTTR_ENABLE(hardware::integrated_circuit)
    };

};
