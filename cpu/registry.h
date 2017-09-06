//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//


#pragma once

#include <hardware/hardware_types.h>
#include <cpu/mc6809/cpu.h>
#include "model.h"
#include "types.h"

namespace ryu::cpu {

    class registry {
    public:
        using cpu_factory = std::function<hardware::cpu* (int, const std::string&)>;

        registry();

        const model_list& models() const;

        hardware::cpu* create(models::ids model, int id, const std::string& name);

    private:
        model_list _models;
        std::map<models::ids, cpu_factory> _factories;
    };

};

