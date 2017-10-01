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

#include <core/context.h>
#include "controller.h"

namespace ryu::emulator {

    class emulator_context : public core::context {
    public:
        struct colors {
            enum indexes {
                black = 0,

                dark_grey = 1,
                fill_color = 1,
            };
        };

        explicit emulator_context(const std::string& name);

    protected:
        void on_initialize() override;

    private:
        void configure_palette();

    private:
        core::palette _palette;
        controller _emulator_state;
    };

};

