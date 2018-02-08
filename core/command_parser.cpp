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
        // -------------------------------------------
        // system commands
        // -------------------------------------------
        {
            "help",
            {
                command::types::help,
                command_size_flags::none,
                {
                    {"cmd", variant::types::string_literal, false}
                },
                "Show help for available commands.",
                "system",
                0
            }
        },

        {
            "!",
            {
                command::types::quit,
                command_size_flags::none,
                {},
                "Exit this application.",
                "system",
                1
            }
        },

        {
            "%",
            {
                command::types::open_editor,
                command_size_flags::none,
                {
                    {"name",  variant::types::string_literal},
                    {"type",  variant::types::identifier, true, false}
                },
                "Execute editor for <italic>name<> and <italic>type<>.",
                "system",
                2
            }
        },

        {
            "clear",
            {
                command::types::clear,
                command_size_flags::none,
                {},
                "Clear console and home the cursor at the top.",
                "system",
                3
            }
        },

        // -------------------------------------------
        // assembler & memory commands
        // -------------------------------------------
        {
            "?",
            {
                command::types::evaluate,
                command_size_flags::byte | command_size_flags::word | command_size_flags::dword,
                {
                    {"...",  variant::types::variadic}
                },
                "Evaluate one or more expressions with optional size specifier.",
                "assembler",
                0
            }
        },

        {
            "^",
            {
                command::types::poke,
                command_size_flags::byte | command_size_flags::word | command_size_flags::dword,
                {
                    {"addr", variant::types::numeric_literal},
                    {"...",  variant::types::variadic}
                },
                "Poke one or more expressions starting at <italic>addr<>.",
                "assembler",
                0
            }
        },

        {
            "@",
            {
                command::types::peek,
                command_size_flags::byte | command_size_flags::word | command_size_flags::dword,
                {
                    {"addr", variant::types::numeric_literal}
                },
                "Peek the value by size specifier at <italic>addr<>.",
                "assembler",
                0
            }
        },

        {
            "a",
            {
                command::types::assemble,
                command_size_flags::none,
                {},
                "Assemble project source files.",
                "assembler",
                1
            }
        },

        {
            "m",
            {
                command::types::memory_editor,
                command_size_flags::none,
                {
                    {"addr", variant::types::numeric_literal}
                },
                "Open the memory hex editor starting at <italic>addr<>.",
                "assembler",
                2
            }
        },

        {
            "d",
            {
                command::types::disassemble,
                command_size_flags::none,
                {
                    {"addr", variant::types::numeric_literal},
                    {"component", variant::types::string_literal},
                },
                "Disassemble program code starting at <italic>addr<> using <italic>component<> as the dialect.",
                "assembler",
                3
            }
        },

        {
            "h",
            {
                command::types::dump_memory,
                command_size_flags::none,
                {
                    {"addr",  variant::types::numeric_literal},
                    {"bytes", variant::types::numeric_literal, false}
                },
                "Dump memory contents at <italic>addr<> to the console; optionally specify the number of <italic>bytes<> (default: 128).",
                "assembler",
                4
            }
        },

        {
            "s",
            {
                command::types::search_memory,
                command_size_flags::byte | command_size_flags::word | command_size_flags::dword,
                {
                    {"addr",  variant::types::numeric_literal},
                    {"count", variant::types::numeric_literal},
                    {"value", variant::types::any}
                },
                "Search memory starting at <italic>addr<> for <italic>count<> of specified size, looking for <italic>value<>.",
                "assembler",
                5
            }
        },

        {
            "f",
            {
                command::types::fill_memory,
                command_size_flags::byte | command_size_flags::word | command_size_flags::dword,
                {
                    {"addr",  variant::types::numeric_literal},
                    {"count", variant::types::numeric_literal},
                    {"value", variant::types::any}
                },
                "Fill memory starting at <italic>addr<> for <italic>count<> of specified size with <italic>value<>.",
                "assembler",
                6
            }
        },

        {
            "c",
            {
                command::types::copy_memory,
                command_size_flags::byte | command_size_flags::word | command_size_flags::dword,
                {
                    {"dest",  variant::types::numeric_literal},
                    {"src",   variant::types::numeric_literal},
                    {"count", variant::types::numeric_literal}
                },
                "Copy memory to <italic>dest<> from <italic>src<> for <italic>count<> of size specifier.",
                "assembler",
                7
            }
        },

        {
            "j",
            {
                command::types::jump_to_address,
                command_size_flags::none,
                {
                    {"addr",  variant::types::numeric_literal, false}
                },
                "Jump to <italic>addr<> and begin execution.",
                "assembler",
                8
            }
        },

        {
            "g",
            {
                command::types::go_to_address,
                command_size_flags::none,
                {
                    {"addr",  variant::types::numeric_literal}
                },
                "Go to <italic>addr<> for execution.",
                "assembler",
                9
            }
        },

        {
            "r",
            {
                command::types::register_editor,
                command_size_flags::none,
                {},
                "Edit the active CPU registers.",
                "assembler",
                10
            }
        },

        {
            "rb",
            {
                command::types::read_binary_to_memory,
                command_size_flags::none,
                {
                    {"path",  variant::types::string_literal},
                    {"start", variant::types::numeric_literal},
                    {"end",   variant::types::numeric_literal, false}
                },
                "Read binary from <italic>path<> to address <italic>start<> to <italic>end<>.",
                "assembler",
                11
            }
        },

        {
            "wb",
            {
                command::types::write_memory_to_binary,
                command_size_flags::none,
                {
                    {"path",  variant::types::string_literal},
                    {"start", variant::types::numeric_literal},
                    {"end",   variant::types::numeric_literal, false}
                },
                "Write binary from address <italic>start<> to <italic>end<> to disk at <italic>path<>.",
                "assembler",
                12
            }
        },

        {
            "t",
            {
                command::types::target,
                command_size_flags::none,
                {
                    {"name", variant::types::string_literal}
                },
                "Find component on active machine matching <italic>name<> and set as target on assembler.",
                "assembler",
                13
            }
        },

        // -------------------------------------------
        // filesystem commands
        // -------------------------------------------
        {
            "rm",
            {
                command::types::remove_file,
                command_size_flags::none,
                {
                    {"path", variant::types::string_literal}
                },
                "Remove file/directory at <italic>path<>.",
                "file system",
                0
            }
        },

        {
            "mv",
            {
                command::types::move_file,
                command_size_flags::none,
                {
                    {"src", variant::types::string_literal},
                    {"dest", variant::types::string_literal}
                },
                "Move the file/directory at path <italic>src<> to path <italic>dest<>.",
                "file system",
                1
            }
        },

        {
            "ls",
            {
                command::types::list_files,
                command_size_flags::none,
                {},
                "List the contents of the current working directory.",
                "file system",
                2
            }
        },

        {
            "cd",
            {
                command::types::change_directory,
                command_size_flags::none,
                {
                    {"path", variant::types::string_literal}
                },
                "Change working directory to <italic>path<>.",
                "file system",
                3
            }
        },

        {
            "pwd",
            {
                command::types::print_working_directory,
                command_size_flags::none,
                {},
                "Show the current working directory.",
                "file system",
                4
            }
        },

        {
            "mkdir",
            {
                command::types::make_directory,
                command_size_flags::none,
                {
                    {"path", variant::types::string_literal}
                },
                "Create a new directory within the current working directory.",
                "file system",
                5
            }
        },

        // -------------------------------------------
        // project commands
        // -------------------------------------------
        {
            "save",
            {
                command::types::save_project_file,
                command_size_flags::none,
                {},
                "Save the currently open file.",
                "project",
                0
            }
        },

        {
            "newproj",
            {
                command::types::new_project,
                command_size_flags::none,
                {
                    {"path", variant::types::string_literal}
                },
                "Create new project at <italic>path<> on disk.",
                "project",
                1
            }
        },

        {
            "ldproj",
            {
                command::types::load_project,
                command_size_flags::none,
                {
                    {"path", variant::types::string_literal, false}
                },
                "Load a project from disk in current working directory or at <italic>path<> into memory.",
                "project",
                2
            }
        },

        // XXX: we can open editors for files via %, but how would we
        //      rename, copy, delete files in the list?
        {
            "lsfiles",
            {
                command::types::list_project_files,
                command_size_flags::none,
                {},
                "List files for currently loaded project.",
                "project",
                3
            }
        },

        {
            "newfile",
            {
                command::types::new_project_file,
                command_size_flags::none,
                {
                    {"path",  variant::types::string_literal},
                    {"type",  variant::types::identifier, false, false}
                },
                "Add a new file to the currently loaded project; optionally specify <italic>type<>.",
                "project",
                4
            }
        },

        {
            "rmfile",
            {
                command::types::remove_project_file,
                command_size_flags::none,
                {
                    {"path", variant::types::string_literal}
                },
                "Removes a project file from the currently loaded project.",
                "project",
                5
            }
        },

        {
            "edproj",
            {
                command::types::edit_project,
                command_size_flags::none,
                {},
                "Open the project editor.",
                "project",
                6
            }
        },

        {
            "saveproj",
            {
                command::types::save_project,
                command_size_flags::none,
                {},
                "Save the currently open project to disk.",
                "project",
                7
            }
        },

        {
            "clproj",
            {
                command::types::close_project,
                command_size_flags::none,
                {},
                "Close the currently open project.",
                "project",
                8
            }
        },

        {
            "cpyproj",
            {
                command::types::clone_project,
                command_size_flags::none,
                {
                    {"path", variant::types::string_literal}
                },
                "Clone the currently open project to <italic>path<> on disk.",
                "project",
                9
            }
        },

        // -------------------------------------------
        // source editor commands
        // -------------------------------------------
        {
            "e",
            {
                command::types::source_editor,
                command_size_flags::none,
                {
                        {"path", variant::types::string_literal}
                },
                "Open the assembly source editor for project file with <italic>name<>.",
                "source editor",
                0
            }
        },

        {
            "rt",
            {
                command::types::read_text,
                command_size_flags::none,
                {
                    {"path", variant::types::string_literal}
                },
                "Read text file at <italic>path<> on disk into editor.",
                "source editor",
                1
            }
        },

        {
            "wt",
            {
                command::types::write_text,
                command_size_flags::none,
                {
                    {"path", variant::types::string_literal}
                },
                "Write text in editor to file at <italic>path<> on disk.",
                "source editor",
                2
            }
        },

        {
            ":",
            {
                command::types::goto_line,
                command_size_flags::none,
                {
                    {"line", variant::types::numeric_literal}
                },
                "Jump to the source <italic>line<>.",
                "source editor",
                3
            }
        },

        {
            // XXX: needs to be fixed in the source editor
            "/",
            {
                command::types::find_text,
                command_size_flags::none,
                {
                    {"needle", variant::types::string_literal}
                },
                "Find <italic>needle<> in text buffer.",
                "source editor",
                4
            }
        },

        // -------------------------------------------
        // machines
        // -------------------------------------------
        {
            "lsmach",
            {
                command::types::list_machines,
                command_size_flags::none,
                {},
                "List machine registry to console.",
                "machine",
                0
            }
        },

        {
            "edmach",
            {
                command::types::edit_machine,
                command_size_flags::none,
                {
                    {"name", variant::types::string_literal}
                },
                "Open machine editor for <italic>name<> machine in registry.",
                "machine",
                1
            }
        },

        {
            "newmach",
            {
                command::types::edit_machine,
                command_size_flags::none,
                {
                    {"name", variant::types::string_literal}
                },
                "Create machine with <italic>name<>.",
                "machine",
                2
            }
        },

        {
            "delmach",
            {
                command::types::delete_machine,
                command_size_flags::none,
                {
                    {"name", variant::types::string_literal}
                },
                "Remove machine <italic>name<> from the registry.",
                "machine",
                3
            }
        },

        {
            "usemach",
            {
                command::types::use_machine,
                command_size_flags::none,
                {
                    {"name", variant::types::string_literal}
                },
                "Make machine <italic>name<> active for the open project.",
                "machine",
                4
            }
        },

        {
            "map",
            {
                command::types::memory_map,
                command_size_flags::none,
                {},
                "Display a memory map for the loaded project's active machine.",
                "machine",
                5
            }
        },

        // -------------------------------------------
        // editor and tool commands
        // -------------------------------------------
        {
            "edsample",
            {
                command::types::sample_editor,
                command_size_flags::none,
                {},
                "Open the sample editor.",
                "sample editor",
                0
            }
        },

        {
            "edmodule",
            {
                command::types::module_editor,
                command_size_flags::none,
                {},
                "Open the module editor.",
                "module editor",
                0
            }
        },

        {
            "edtiles",
            {
                command::types::tile_editor,
                command_size_flags::none,
                {},
                "Open the tile editor.",
                "tile editor",
                0
            }
        },

        {
            "edsprites",
            {
                command::types::sprite_editor,
                command_size_flags::none,
                {},
                "Open the sprite editor.",
                "sprite editor",
                0
            }
        },

        {
            "edbg",
            {
                command::types::background_editor,
                command_size_flags::none,
                {},
                "Open the backgrounds editor.",
                "background editor",
                0
            }
        },

        // -------------------------------------------
        // environment commands
        // -------------------------------------------
        {
            "lsenv",
            {
                command::types::list_environments,
                command_size_flags::none,
                {},
                "List environments for currently loaded project.",
                "environment",
                0
            }
        },

        {
            "newenv",
            {
                command::types::new_environment,
                command_size_flags::none,
                {
                    {"name",  variant::types::string_literal}
                },
                "Add a new environment using <italic>name<> to currently loaded project.",
                "environment",
                1
            }
        },

        {
            "rmenv",
            {
                command::types::remove_environment,
                command_size_flags::none,
                {
                    {"name", variant::types::string_literal}
                },
                "Remove environment using <italic>name<> from currently loaded project.",
                "environment",
                2
            }
        },

        {
            "edenv",
            {
                command::types::edit_environment,
                command_size_flags::none,
                {
                    {"name", variant::types::string_literal, false}
                },
                "Open environment in source editor.",
                "environment",
                3
            }
        },

        {
            "switchenv",
            {
                command::types::switch_environment,
                command_size_flags::none,
                {
                    {"name", variant::types::string_literal, false}
                },
                "Switch to environment using <italic>name<>.  If <italic>name<> is omitted, no environment is loaded. NOTE: Clears symbol table before load.",
                "environment",
                4
            }
        },

        {
            "applyenv",
            {
                command::types::apply_environment,
                command_size_flags::none,
                {
                },
                "Apply currently active project environment.",
                "environment",
                5
            }
        },

        // -------------------------------------------
        // symbol table commands
        // -------------------------------------------
        {
            "set",
            {
                command::types::add_symbol,
                command_size_flags::none,
                {
                    {"name", variant::types::identifier, true, false},
                    {"value", variant::types::any,       true, false}
                },
                "Assign the result of <italic>value<> to the symbol of <italic>name<>.",
                "symbols",
                0
            }
        },

        {
            "unset",
            {
                command::types::remove_symbol,
                command_size_flags::none,
                {
                    {"name", variant::types::identifier, true, false}
                },
                "Remove the symbol <italic>name<> from the symbol table.",
                "symbols",
                1
            }
        },

        {
            "symbols",
            {
                command::types::show_symbol_table,
                command_size_flags::none,
                {},
                "List the symbol table contents to the console.",
                "symbols",
                2
            }
        },
    };

    command_table& command_parser::command_catalog() {
        return _commands;
    }

    command_parser::command_parser() : parser() {
    }

    ast_node_shared_ptr command_parser::parse_command() {
        consume_white_space();

        auto token = current_token();
        if (token == nullptr)
            return nullptr;

        if (!isspace(*token)) {
            std::stringstream stream;
            auto size = command_size_flags::none;
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
                        std::string size_token;
                        if (*token == 'b')
                            size_token = "b";
                        else if (*token == 'w')
                            size_token = "w";
                        else if (*token == 'd') {
                            size_token = "d";
                            token = move_to_next_token();
                            if (token == nullptr) {
                                error("P008", "unexpected end of input");
                                return nullptr;
                            }
                            if (*token == 'w') {
                                size_token += "w";
                            }
                        } else {
                            error("P011", "invalid size suffix");
                            return nullptr;
                        }
                        size = command_t::token_to_size(size_token);
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

            auto command_spec = it->second;
            if (size != command_size_flags::none
            && (command_spec.valid_sizes & size) == 0) {
                error("P011", "invalid size specifier for command");
                return nullptr;
            }

            auto command_node = create_ast_node(ast_node_t::tokens::command);
            command_node->value = command_t {
                    command_spec,
                    stream.str(),
                    size};

            auto statement_node = create_ast_node(ast_node_t::tokens::statement);
            statement_node->rhs = create_ast_node(ast_node_t::tokens::parameter_list);

            command_node->lhs = statement_node;
            command_node->rhs = create_ast_node(ast_node_t::tokens::parameter_list);

            while (true) {
                auto expr = parse_expression();
                if (expr == nullptr)
                    break;
                statement_node->rhs->children.push_back(expr);
            }

            consume_white_space();

            token = current_token();
            if (token != nullptr) {
                if (*token == '@') {
                    token = move_to_next_token();
                    if (token == nullptr) {
                        error("P011", "expected redirection operator");
                        return nullptr;
                    }

                    if (*token == '|') {
                        token = move_to_next_token();
                        if (token == nullptr) {
                            error("P011", "expected pipe destination");
                            return nullptr;
                        }

                        consume_white_space();

                        auto identifier = parse_identifier();
                        if (identifier == nullptr) {
                            error("P011", "expected pipe destination");
                            return nullptr;
                        }

                        auto redirection_node = create_ast_node(ast_node_t::tokens::redirection);
                        redirection_node->lhs = create_ast_node(ast_node_t::tokens::string_literal);
                        redirection_node->lhs->value = string_literal_t{"|"};
                        redirection_node->rhs = identifier;
                        command_node->rhs->children.push_back(redirection_node);
                    } else if (*token == '>') {
                        std::string symbol(">");

                        token = move_to_next_token();
                        if (token != nullptr && *token == '>') {
                            symbol += ">";
                            token = move_to_next_token();
                        }

                        if (token == nullptr) {
                            error("P011", "expected path string literal");
                            return nullptr;
                        }

                        consume_white_space();

                        auto string_literal = parse_string_literal();
                        if (string_literal == nullptr) {
                            error("P011", "expected path string literal");
                            return nullptr;
                        }

                        auto redirection_node = create_ast_node(ast_node_t::tokens::redirection);
                        redirection_node->lhs = create_ast_node(ast_node_t::tokens::string_literal);
                        redirection_node->lhs->value = string_literal_t{symbol};
                        redirection_node->rhs = string_literal;
                        command_node->rhs->children.push_back(redirection_node);
                    }
                }
            }

            return is_failed() ? nullptr : command_node;
        }

        return nullptr;
    }

    ast_node_shared_ptr command_parser::parse(const parser_input_t& input) {
        reset(input);
        return parse_command();
    }

}