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
#include <core/project.h>
#include "console/controller.h"
#include "hex_editor/controller.h"
#include "text_editor/controller.h"
#include "machine_editor/controller.h"

namespace ryu::ide {

    class context : public core::context {
    public:
        struct colors {
            enum indexes {
                black = 0,

                white = 1,

                grey = 2,

                blue = 3,
                fill_color = 3,

                red = 4,

                cyan = 5,

                purple = 6,

                green = 7,

                yellow = 8,

                orange = 9,

                brown = 10,

                pink = 11,

                dark_grey = 12,

                light_green = 13,

                light_blue = 14,
                selection = 14,
                caret = 14,
                text = 14,

                light_grey = 15,
                info_text = 15,

                transparent = 16
            };
        };

        explicit context(std::string name);

        core::project* project();

        void project(core::project* project);

    protected:
        void on_initialize() override;

    private:
        core::palette _palette;
        core::project* _project = nullptr;
        console::controller _console_state;
        hex_editor::controller _hex_editor_state;
        text_editor::controller _text_editor_state;
        machine_editor::controller _machine_editor_state;
    };

};

