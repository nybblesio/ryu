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

        core::context_window::sizes size() const;

        void size(core::context_window::sizes value);

        void parent_resize(const core::rect& parent_bounds);

    protected:
        void on_draw(core::renderer& surface) override;

        bool on_initialize(core::result& result) override;

    private:
        void bind_events();

        void configure_states();

        void configure_palette();

    private:
        hex_editor::controller _hex_editor_state;
        console_editor::controller _console_state;
        source_editor::controller _source_editor_state;
        machine_editor::controller _machine_editor_state;
        core::context_window::sizes _size = core::context_window::sizes::split;
    };

};

