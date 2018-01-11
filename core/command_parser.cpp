//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "command_parser.h"

namespace ryu::core {

    command_table command_parser::_commands = {
        // system commands
        {
            "help",
            {
                command_types::help,
                {},
                "Show help for available commands"
            }
        },

        // assembler & memory commands
        {
            "!",
            {
                command_types::quit,
                {},
                "Quit Ryu: The Arcade Construction Kit"
            }
        },

        {
            "?",
            {
                command_types::evaluate,
                {
                    {"...",  variant::types::variadic}
                },
                "expression evaluator, where [size] can be .b .w .dw"
            }
        },

        {
            "a",
            {
                command_types::assemble,
                {
                    {"addr", variant::types::numeric_literal, false}
                },
                "TODO: put help text in here"
            }
        },

        {
            "e",
            {
                command_types::source_editor,
                {
                    {"name", variant::types::string_literal, false}
                },
                "open the text editor"
            }
        },

        {
            "m",
            {
                command_types::memory_editor,
                {
                    {"addr", variant::types::numeric_literal}
                },
                "open hex editor"
            }
        },

        {
            "d",
            {
                command_types::disassemble,
                {
                    {"addr", variant::types::numeric_literal}
                },
                "disassemble"
            }
        },

        {
            "h",
            {
                command_types::hex_dump,
                {
                    {"addr", variant::types::numeric_literal}
                },
                "hex dump into console"
            }
        },

        {
            "s",
            {
                command_types::search_memory,
                {
                    {"addr",  variant::types::numeric_literal},
                    {"bytes", variant::types::numeric_literal},
                    {"value", variant::types::any}
                },
                "search memory"
            }
        },

        {
            "f",
            {
                command_types::fill_memory,
                {
                    {"addr",  variant::types::numeric_literal},
                    {"bytes", variant::types::numeric_literal},
                    {"value", variant::types::any}
                },
                "fill memory"
            }
        },

        {
            "c",
            {
                command_types::copy_memory,
                {
                    {"dest",  variant::types::numeric_literal},
                    {"src",   variant::types::numeric_literal},
                    {"bytes", variant::types::numeric_literal}
                },
                "copy memory"
            }
        },

        {
            "j",
            {
                command_types::jump_to_address,
                {
                    {"addr",  variant::types::numeric_literal, false}
                },
                "jump to address and begin execution"
            }
        },

        {
            "g",
            {
                command_types::go_to_address,
                {
                    {"addr",  variant::types::numeric_literal, false}
                },
                "go to address"
            }
        },

        {
            "r",
            {
                command_types::register_editor,
                {},
                "view/edit CPU registers"
            }
        },

        {
            "rb",
            {
                command_types::read_binary_to_memory,
                {
                    {"path",  variant::types::string_literal},
                    {"start", variant::types::numeric_literal},
                    {"end",   variant::types::numeric_literal, false}
                },
                "read binary into memory"
            }
        },

        {
            "wb",
            {
                command_types::write_memory_to_binary,
                {
                    {"path",  variant::types::string_literal},
                    {"start", variant::types::numeric_literal},
                    {"end",   variant::types::numeric_literal, false}
                },
                "write memory to binary file on disk"
            }
        },

        // filesystem commands
        {
            "rm",
            {
                command_types::remove_file,
                {
                    {"path", variant::types::string_literal}
                },
                "TODO: put help text in here"
            }
        },

        {
            "dir",
            {
                command_types::dir,
                {},
                "TODO: put help text in here"
            }
        },

        {
            "cd",
            {
                command_types::change_directory,
                {
                    {"path", variant::types::string_literal}
                },
                "TODO: put help text in here"
            }
        },

        {
            "pwd",
            {
                command_types::print_working_directory,
                {},
                "TODO: put help text in here"
            }
        },

        {
            "clear",
            {
                command_types::clear,
                {},
                "TODO: put help text in here"
            }
        },

        // project commands
        {
            "new",
            {
                command_types::new_project,
                {
                    {"path", variant::types::string_literal}
                },
                "TODO: put help text in here"
            }
        },

        {
            "load",
            {
                command_types::load_project,
                {
                    {"path", variant::types::string_literal}
                },
                "TODO: put help text in here"
            }
        },

        {
            "save",
            {
                command_types::save_project,
                {},
                "TODO: put help text in here"
            }
        },

        {
            "clone",
            {
                command_types::clone_project,
                {
                    {"path", variant::types::string_literal}
                },
                "TODO: put help text in here"
            }
        },

        // text editor commands
        {
            "rt",
            {
                command_types::read_text,
                {
                    {"path",   variant::types::string_literal}
                },
                "read text file into editor buffer"
            }
        },

        {
            "wt",
            {
                command_types::write_text,
                {
                    {"path",   variant::types::string_literal}
                },
                "write text in buffer to file"
            }
        },

        {
            ":",
            {
                command_types::goto_line,
                {
                    {"line",   variant::types::numeric_literal}
                },
                "goto line number"
            }
        },

        {
            "/",
            {
                command_types::find_text,
                {
                    {"needle", variant::types::string_literal}
                },
                "find needle in buffer"
            }
        },

        // machines
        {
            "machines",
            {
                command_types::machines_list,
                {},
                "TODO: put help text in here"
            }
        },

        {
            "machine",
            {
                command_types::machine_editor,
                {
                    {"name", variant::types::string_literal}
                },
                "TODO: put help text in here"
            }
        },

        {
            "delmachine",
            {
                command_types::del_machine,
                {
                    {"name", variant::types::string_literal}
                },
                "TODO: put help text in here"
            }
        },

        {
            "usemachine",
            {
                command_types::use_machine,
                {
                    {"name", variant::types::string_literal}
                },
                "TODO: put help text in here"
            }
        },

        // editor and tool commands
        {
            "%",
            {
                command_types::open_editor,
                {
                    {"name",  variant::types::string_literal},
                    {"type",  variant::types::identifier, true, false}
                },
                "TODO: put help text in here"
            }
        },

        {
            "sounds",
            {
                command_types::sounds,
                {},
                "TODO: put help text in here"
            }
        },

        {
            "tracker",
            {
                command_types::tracker,
                {},
                "TODO: put help text in here"
            }
        },

        {
            "tiles",
            {
                command_types::tile_editor,
                {},
                "TODO: put help text in here"
            }
        },

        {
            "sprites",
            {
                command_types::sprite_editor,
                {},
                "TODO: put help text in here"
            }
        },

        {
            "backgrounds",
            {
                command_types::background_editor,
                {},
                "TODO: put help text in here"
            }
        },

        // symbol table commands
        {
            "set",
            {
                command_types::add_symbol,
                {
                    {"name", variant::types::identifier, true, false},
                    {"value", variant::types::any,       true, false}
                },
                "assign the result of expression to the symbol of [name]"
            }
        },

        {
            "unset",
            {
                command_types::remove_symbol,
                {
                    {"name", variant::types::identifier, true, false}
                },
                "removes the symbol [name] from the table"
            }
        },

        {
            "symbols",
            {
                command_types::show_symbol_table,
                {},
                "dumps the symbol table"
            }
        },
    };

    command_table& command_parser::command_catalog() {
        return _commands;
    }

    command_parser::command_parser() : parser() {
    }

    ast_node_shared_ptr command_parser::parse(const std::string& input) {
        reset(input);
        return parse_command();
    }

    ast_node_shared_ptr command_parser::parse_command() {
        consume_white_space();
        auto token = current_token();
        if (!isspace(*token)) {
            std::stringstream stream;
            auto size = command_t::sizes::dword;
            stream << *token;
            while (true) {
                token = move_to_next_token();
                if (token == nullptr)
                    break;
                if (!isspace(*token)) {
                    if (*token == '.') {
                        token = move_to_next_token();
                        if (token == nullptr) {
                            error("P008", "unexpected end of input");
                            return nullptr;
                        }
                        if (*token == 'b')
                            size = command_t::sizes::byte;
                        else if (*token == 'w')
                            size = command_t::sizes::word;
                        else if (*token == 'd') {
                            token = move_to_next_token();
                            if (token == nullptr) {
                                error("P008", "unexpected end of input");
                                return nullptr;
                            }
                            if (*token == 'w') {
                                size = command_t::sizes::dword;
                            }
                        } else {
                            error("P011", "invalid size suffix");
                            return nullptr;
                        }
                    } else {
                        stream << *token;
                    }
                }
                else
                    break;
            }
            auto it = _commands.find(stream.str());
            if (it == _commands.end()) {
                error("P010", "unknown command");
                return nullptr;
            }
            auto command_node = std::make_shared<ast_node_t>();
            command_node->value = command_t {it->second, stream.str(), size};
            command_node->token = ast_node_t::tokens::command;

            while (true) {
                auto expr = parse_expression();
                if (expr == nullptr)
                    break;
                command_node->children.push_back(expr);
            }

            return is_failed() ? nullptr : command_node;
        }
        return nullptr;
    }

}