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
#include "core/environment.h"
#include "hex_editor/controller.h"
#include "source_editor/controller.h"
#include "console_editor/controller.h"
#include "machine_editor/controller.h"

namespace ryu::ide {

    class ide_context : public core::context {
    public:
        explicit ide_context(const std::string& name);

        core::project* project();

        void project(core::project* project);

    protected:
        bool on_initialize(core::result& result) override;

    private:
        void configure_palette();

    private:
        core::palette _palette;
        core::project* _project = nullptr;
        hex_editor::controller _hex_editor_state;
        console_editor::controller _console_state;
        source_editor::controller _source_editor_state;
        machine_editor::controller _machine_editor_state;
    };

};

