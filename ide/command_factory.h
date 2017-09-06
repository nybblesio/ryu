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

#include <core/result.h>

namespace ryu::ide {

    class command_factory {
    public:
        struct command {
            enum types : uint8_t {
                quit = 1,
                assemble,
                evaluate,
                text_editor,
                memory_editor,
                disassemble,
                hex_dump,
                search_memory,
                fill_memory,
                copy_memory,
                jump_to_address,
                go_to_address,
                register_editor,
                clear,
                change_directory,
                print_working_directory,
                new_project,
                load_project,
                save_project,
                clone_project,
                remove_file,
                dir,
                machines_editor,
                set_machine,
                sprite_editor,
                tile_editor,
                background_editor,
                tracker,
                sounds,
                read_binary_to_memory,
                write_memory_to_binary,
                read_text,
                write_text,
                goto_line,
                find_text
            };
        };

        command_factory() = default;

        ~command_factory() = default;

        bool execute(core::result& result, const std::string& line);
    };

};

