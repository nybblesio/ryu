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
                text = 1,
                caret = 1,
                selection = 1,

                grey = 2,
                info_text = 2,

                dark_blue = 3,
                fill_color = 3,
            };
        };

        struct states {
            enum id {
                text_editor = 1,
                console,
                hex_editor,
                machine_editor,
            };
        };

        context(int id, std::string name);

        core::project* project();

        void project(core::project* project);

    private:
        core::palette _palette;
        core::project* _project = nullptr;
        console::controller _console_state;
        hex_editor::controller _hex_editor_state;
        text_editor::controller _text_editor_state;
        machine_editor::controller _machine_editor_state;
    };

};

