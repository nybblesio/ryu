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
        explicit emulator_context(const std::string& name);

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
        controller _emulator_state;
        core::context_window::sizes _size = core::context_window::sizes::split;
    };

};

