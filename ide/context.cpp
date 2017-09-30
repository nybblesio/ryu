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

    context::context(std::string name) : core::context(name),
                                         _palette(),
                                         _console_state(this, "console"),
                                         _hex_editor_state(this, "hex editor"),
                                         _text_editor_state(this, "text editor"),
                                         _machine_editor_state(this, "machine editor") {
        add_state(
                &_console_state,
                [this](auto state, auto& command, auto& params) {
                    if (command == "text_editor") {
                        push_state(_text_editor_state.id(), params);
                        return true;
                    } else if (command == "hex_editor") {
                        push_state(_hex_editor_state.id(), params);
                        return true;
                    } else if (command == "machine_editor") {
                        push_state(_machine_editor_state.id(), params);
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
        grey.red(0x78);
        grey.green(0x78);
        grey.blue(0x78);
        grey.alpha(0xff);

        auto& blue = _palette[colors::blue];
        blue.red(0x40);
        blue.green(0x31);
        blue.blue(0x8c);
        blue.alpha(0xff);

        auto& red = _palette[colors::red];
        red.red(0x88);
        red.green(0x33);
        red.blue(0x32);
        red.alpha(0xff);
        
        auto& cyan = _palette[colors::cyan];
        cyan.red(0x67);
        cyan.green(0xb6);
        cyan.blue(0xbd);
        cyan.alpha(0xff);

        auto& purple = _palette[colors::purple];
        purple.red(0x8b);
        purple.green(0x3f);
        purple.blue(0x97);
        purple.alpha(0xff);
        
        auto& green = _palette[colors::green];
        green.red(0x57);
        green.green(0xa0);
        green.blue(0x49);
        green.alpha(0xff);

        auto& yellow = _palette[colors::yellow];
        yellow.red(0xbf);
        yellow.green(0xce);
        yellow.blue(0x73);
        yellow.alpha(0xff);

        auto& orange = _palette[colors::orange];
        orange.red(0x8b);
        orange.green(0x55);
        orange.blue(0x29);
        orange.alpha(0xff);

        auto& brown = _palette[colors::brown];
        brown.red(0x57);
        brown.green(0x42);
        brown.blue(0x00);
        brown.alpha(0xff);

        auto& pink = _palette[colors::pink];
        pink.red(0xb8);
        pink.green(0x69);
        pink.blue(0x62);
        pink.alpha(0xff);

        auto& dark_grey = _palette[colors::dark_grey];
        dark_grey.red(0x50);
        dark_grey.green(0x50);
        dark_grey.blue(0x50);
        dark_grey.alpha(0xff);

        auto& light_green = _palette[colors::light_green];
        light_green.red(0x95);
        light_green.green(0xe0);
        light_green.blue(0x88);
        light_green.alpha(0xff);

        auto& light_blue = _palette[colors::light_blue];
        light_blue.red(0x78);
        light_blue.green(0x69);
        light_blue.blue(0xc4);
        light_blue.alpha(0xff);

        auto& light_grey = _palette[colors::light_grey];
        light_grey.red(0x9f);
        light_grey.green(0x9f);
        light_grey.blue(0x9f);
        light_grey.alpha(0xff);

        palette(&_palette);
    }

    void context::on_initialize() {
        push_state(_console_state.id(), {});
    }

    core::project* context::project() {
        return _project;
    }

    void context::project(core::project* project) {
        _project = project;
    }

}