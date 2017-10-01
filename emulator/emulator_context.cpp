//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "emulator_context.h"

namespace ryu::emulator {

    emulator_context::emulator_context(const std::string& name) : core::context(name),
                                                                  _palette(),
                                                                  _emulator_state("execute") {
    }

    void emulator_context::on_initialize() {
        configure_palette();
        add_state(&_emulator_state);
        push_state(_emulator_state.id(), {});
    }

    void emulator_context::configure_palette() {
        auto& black = _palette[colors::indexes::black];
        black.red(0x00);
        black.green(0x00);
        black.blue(0x00);
        black.alpha(0xff);

        auto& dark_grey = _palette[colors::indexes::dark_grey];
        dark_grey.red(0x65);
        dark_grey.green(0x68);
        dark_grey.blue(0x6d);
        dark_grey.alpha(0xff);

        palette(&_palette);
    }
}