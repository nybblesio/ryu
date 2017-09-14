//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "context.h"

namespace ryu::ide {

    context::context(int id, std::string name) : core::context(id, name),
                                                 _palette(),
                                                 _console_state(this, states::console, "console"),
                                                 _hex_editor_state(this, states::hex_editor, "hex editor"),
                                                 _text_editor_state(this, states::text_editor, "text editor"),
                                                 _machine_editor_state(this, states::machine_editor, "machine editor") {
        add_state(
                &_console_state,
                [this](auto state, auto& command) {
                    if (command == "text_editor") {
                        push_state(states::text_editor);
                        return true;
                    } else if (command == "hex_editor") {
                        push_state(states::hex_editor);
                        return true;
                    } else if (command == "machine_editor") {
                        push_state(states::machine_editor);
                        return true;
                    }
                    return false;
                });

        add_state(&_hex_editor_state);
        add_state(&_text_editor_state);
        add_state(&_machine_editor_state);

        auto& black = _palette[colors::black];
        black.red(0x00);
        black.green(0x00);
        black.blue(0x00);
        black.alpha(0xff);

        auto& white = _palette[colors::white];
        white.red(0xff);
        white.green(0xff);
        white.blue(0xff);
        white.alpha(0xff);

        auto& grey = _palette[colors::grey];
        grey.red(0x93);
        grey.green(0x93);
        grey.blue(0x93);
        grey.alpha(0xff);

        auto& dark_blue = _palette[colors::dark_blue];
        dark_blue.red(0x00);
        dark_blue.green(0x00);
        dark_blue.blue(0x68);
        dark_blue.alpha(0xff);

        palette(&_palette);
    }

    core::project* context::project() {
        return _project;
    }

    void context::project(core::project* project) {
        _project = project;
    }
}