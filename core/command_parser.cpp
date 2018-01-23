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
                command_types::help,
                command_size_flags::none,
                {
                    {"cmd", variant::types::command_literal, false}
                },
                "Show help for available commands."
            }
        },

        {
            "!",
            {
                command_types::quit,
                command_size_flags::none,
                {},
                "Exit this application."
            }
        },

        {
            "%",
            {
                command_types::open_editor,
                command_size_flags::none,
                {
                    {"name",  variant::types::string_literal},
                    {"type",  variant::types::identifier, true, false}
                },
                "Execute editor for <italic>name<> and <italic>type<>."
            }
        },

        {
            "clear",
            {
                command_types::clear,
                command_size_flags::none,
                {},
                "Clear console and home the cursor at the top."
            }
        },

        // -------------------------------------------
        // assembler & memory commands
        // -------------------------------------------
        {
            "?",
            {
                command_types::evaluate,
                command_size_flags::byte | command_size_flags::word | command_size_flags::dword,
                {
                    {"...",  variant::types::variadic}
                },
                "Evaluate one or more expressions with optional size specifier."
            }
        },

        {
            "a",
            {
                command_types::assemble,
                command_size_flags::none,
                {
                    {"addr", variant::types::numeric_literal, false}
                },
                "Assemble project source files, optionally at target <italic>addr<>."
            }
        },

        {
            "e",
            {
                command_types::source_editor,
                command_size_flags::none,
                {
                    {"name", variant::types::string_literal, false}
                },
                "Open the assembly source editor, optionally for file with <italic>name<>."
            }
        },

        {
            "m",
            {
                command_types::memory_editor,
                command_size_flags::none,
                {
                    {"addr", variant::types::numeric_literal}
                },
                "Open the memory hex editor starting at <italic>addr<>."
            }
        },

        {
            // XXX: how do we know what CPU type to use for disassembly
            "d",
            {
                command_types::disassemble,
                command_size_flags::none,
                {
                    {"addr", variant::types::numeric_literal}
                },
                "Disassemble program code starting at <italic>addr<>."
            }
        },

        {
            "h",
            {
                command_types::dump_memory,
                command_size_flags::none,
                {
                    {"addr", variant::types::numeric_literal}
                },
                "Dump memory contents at <italic>addr<> to the console."
            }
        },

        {
            // XXX: what should the result of this command be?
            "s",
            {
                command_types::search_memory,
                command_size_flags::byte | command_size_flags::word | command_size_flags::dword,
                {
                    {"addr",  variant::types::numeric_literal},
                    {"bytes", variant::types::numeric_literal},
                    {"value", variant::types::any}
                },
                "Search memory starting at <italic>addr<> for <italic>bytes<> looking for <italic>value<>."
            }
        },

        {
            "f",
            {
                command_types::fill_memory,
                command_size_flags::none,
                {
                    {"addr",  variant::types::numeric_literal},
                    {"bytes", variant::types::numeric_literal},
                    {"value", variant::types::any}
                },
                "Fill memory starting at <italic>addr<> for <italic>bytes<> with <italic>value<>."
            }
        },

        {
            "c",
            {
                command_types::copy_memory,
                command_size_flags::none,
                {
                    {"dest",  variant::types::numeric_literal},
                    {"src",   variant::types::numeric_literal},
                    {"bytes", variant::types::numeric_literal}
                },
                "Copy memory to <italic>dest<> from <italic>src<> for <italic>bytes<>."
            }
        },

        {
            "j",
            {
                command_types::jump_to_address,
                command_size_flags::none,
                {
                    {"addr",  variant::types::numeric_literal, false}
                },
                "Jump to <italic>addr<> and begin execution."
            }
        },

        {
            "g",
            {
                command_types::go_to_address,
                command_size_flags::none,
                {
                    {"addr",  variant::types::numeric_literal}
                },
                "Go to <italic>addr<> for execution."
            }
        },

        {
            "r",
            {
                command_types::register_editor,
                command_size_flags::none,
                {},
                "Edit the active CPU registers."
            }
        },

        {
            "rb",
            {
                command_types::read_binary_to_memory,
                command_size_flags::none,
                {
                    {"path",  variant::types::string_literal},
                    {"start", variant::types::numeric_literal},
                    {"end",   variant::types::numeric_literal, false}
                },
                "Read binary from <italic>path<> to address <italic>start<> to <italic>end<>."
            }
        },

        {
            "wb",
            {
                command_types::write_memory_to_binary,
                command_size_flags::none,
                {
                    {"path",  variant::types::string_literal},
                    {"start", variant::types::numeric_literal},
                    {"end",   variant::types::numeric_literal, false}
                },
                "Write binary from address <italic>start<> to <italic>end<> to disk at <italic>path<>."
            }
        },

        // -------------------------------------------
        // filesystem commands
        // -------------------------------------------
        {
            "rm",
            {
                command_types::remove_file,
                command_size_flags::none,
                {
                    {"path", variant::types::string_literal}
                },
                "Remove file/directory at <italic>path<>."
            }
        },

        {
            "mv",
            {
                command_types::move_file,
                command_size_flags::none,
                {
                    {"src", variant::types::string_literal},
                    {"dest", variant::types::string_literal}
                },
                "Move the file/directory at path <italic>src<> to path <italic>dest<>."
            }
        },

        {
            "ls",
            {
                command_types::list_files,
                command_size_flags::none,
                {},
                "List the contents of the current working directory."
            }
        },

        {
            "cd",
            {
                command_types::change_directory,
                command_size_flags::none,
                {
                    {"path", variant::types::string_literal}
                },
                "Change working directory to <italic>path<>."
            }
        },

        {
            "pwd",
            {
                command_types::print_working_directory,
                command_size_flags::none,
                {},
                "Show the current working directory."
            }
        },

        {
            "mkdir",
            {
                command_types::make_directory,
                command_size_flags::none,
                {
                    {"path", variant::types::string_literal}
                },
                "Create a new directory within the current working directory."
            }
        },

        // -------------------------------------------
        // project commands
        // -------------------------------------------
        {
            "newproj",
            {
                command_types::new_project,
                command_size_flags::none,
                {
                    {"path", variant::types::string_literal}
                },
                "Create new project at <italic>path<> on disk."
            }
        },

        {
            "ldproj",
            {
                command_types::load_project,
                command_size_flags::none,
                {
                    {"path", variant::types::string_literal, false}
                },
                "Load a project from disk in current working directory or at <italic>path<> into memory."
            }
        },

        // XXX: we can open editors for files via %, but how would we
        //      rename, copy, delete files in the list?
        {
            "lsfiles",
            {
                command_types::list_project_files,
                command_size_flags::none,
                {},
                "List files for currently loaded project."
            }
        },

        {
            "newfile",
            {
                    command_types::new_project_file,
                    command_size_flags::none,
                    {
                        {"path",  variant::types::string_literal},
                        {"type",  variant::types::identifier, false, false}
                    },
                    "Add a new file to the currently loaded project; optionally specify <italic>type<>."
            }
        },

        {
            "rmfile",
            {
                    command_types::remove_project_file,
                    command_size_flags::none,
                    {
                        {"path", variant::types::string_literal}
                    },
                    "Removes a project file from the currently loaded project."
            }
        },

        {
            "edproj",
            {
                command_types::edit_project,
                command_size_flags::none,
                {},
                "Open the project editor."
            }
        },

        {
            "saveproj",
            {
                command_types::save_project,
                command_size_flags::none,
                {},
                "Save the currently open project to disk."
            }
        },

        {
            "clproj",
            {
                command_types::close_project,
                command_size_flags::none,
                {},
                "Close the currently open project."
            }
        },

        {
            "cpyproj",
            {
                command_types::clone_project,
                command_size_flags::none,
                {
                    {"path", variant::types::string_literal}
                },
                "Clone the currently open project to <italic>path<> on disk."
            }
        },

        // -------------------------------------------
        // source editor commands
        // -------------------------------------------
        {
            "rt",
            {
                command_types::read_text,
                command_size_flags::none,
                {
                    {"path", variant::types::string_literal}
                },
                "Read text file at <italic>path<> on disk into editor."
            }
        },

        {
            "wt",
            {
                command_types::write_text,
                command_size_flags::none,
                {
                    {"path", variant::types::string_literal}
                },
                "Write text in editor to file at <italic>path<> on disk."
            }
        },

        {
            ":",
            {
                command_types::goto_line,
                command_size_flags::none,
                {
                    {"line", variant::types::numeric_literal}
                },
                "Jump to the source <italic>line<>."
            }
        },

        {
            // XXX: needs to be fixed in the source editor
            "/",
            {
                command_types::find_text,
                command_size_flags::none,
                {
                    {"needle", variant::types::string_literal}
                },
                "Find <italic>needle<> in text buffer."
            }
        },

        // -------------------------------------------
        // machines
        // -------------------------------------------
        {
            "lsmach",
            {
                command_types::list_machines,
                command_size_flags::none,
                {},
                "List machine registry to console."
            }
        },

        {
            "edmach",
            {
                command_types::edit_machine,
                command_size_flags::none,
                {
                    {"name", variant::types::string_literal}
                },
                "Open machine editor for <italic>name<> machine in registry."
            }
        },

        {
            "newmach",
            {
                command_types::edit_machine,
                command_size_flags::none,
                {
                    {"name", variant::types::string_literal}
                },
                "Create machine with <italic>name<>."
            }
        },

        {
            "delmach",
            {
                command_types::delete_machine,
                command_size_flags::none,
                {
                    {"name", variant::types::string_literal}
                },
                "Remove machine <italic>name<> from the registry."
            }
        },

        {
            "usemach",
            {
                command_types::use_machine,
                command_size_flags::none,
                {
                    {"name", variant::types::string_literal}
                },
                "Make machine <italic>name<> active for the open project."
            }
        },

        // -------------------------------------------
        // editor and tool commands
        // -------------------------------------------
        {
            "edsample",
            {
                command_types::sample_editor,
                command_size_flags::none,
                {},
                "Open the sample editor."
            }
        },

        {
            "edmodule",
            {
                command_types::module_editor,
                command_size_flags::none,
                {},
                "Open the module editor."
            }
        },

        {
            "edtiles",
            {
                command_types::tile_editor,
                command_size_flags::none,
                {},
                "Open the tile editor."
            }
        },

        {
            "edsprites",
            {
                command_types::sprite_editor,
                command_size_flags::none,
                {},
                "Open the sprite editor."
            }
        },

        {
            "edbg",
            {
                command_types::background_editor,
                command_size_flags::none,
                {},
                "Open the backgrounds editor."
            }
        },

        // -------------------------------------------
        // environment commands
        // -------------------------------------------
        {
            "lsenv",
            {
                command_types::list_environments,
                command_size_flags::none,
                {},
                "List environments for currently loaded project."
            }
        },

        {
            "newenv",
            {
                command_types::new_environment,
                command_size_flags::none,
                {
                    {"name",  variant::types::string_literal}
                },
                "Add a new environment using <italic>name<> to currently loaded project."
            }
        },

        {
            "rmenv",
            {
                command_types::remove_environment,
                command_size_flags::none,
                {
                    {"name", variant::types::string_literal}
                },
                "Remove environment using <italic>name<> from currently loaded project."
            }
        },

        {
            "switchenv",
            {
                command_types::switch_environment,
                command_size_flags::none,
                {
                    {"name", variant::types::string_literal}
                },
                "Switch to environment using <italic>name<>. NOTE: Clears symbol table before load."
            }
        },

        // -------------------------------------------
        // symbol table commands
        // -------------------------------------------
        {
            "set",
            {
                command_types::add_symbol,
                command_size_flags::none,
                {
                    {"name", variant::types::identifier, true, false},
                    {"value", variant::types::any,       true, false}
                },
                "Assign the result of <italic>value<> to the symbol of <italic>name<>."
            }
        },

        {
            "unset",
            {
                command_types::remove_symbol,
                command_size_flags::none,
                {
                    {"name", variant::types::identifier, true, false}
                },
                "Remove the symbol <italic>name<> from the symbol table."
            }
        },

        {
            "symbols",
            {
                command_types::show_symbol_table,
                command_size_flags::none,
                {},
                "List the symbol table contents to the console."
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
                        if (*token == 'b')
                            size = command_size_flags::byte;
                        else if (*token == 'w')
                            size = command_size_flags::word;
                        else if (*token == 'd') {
                            token = move_to_next_token();
                            if (token == nullptr) {
                                error("P008", "unexpected end of input");
                                return nullptr;
                            }
                            if (*token == 'w') {
                                size = command_size_flags::dword;
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

            auto command_spec = it->second;
            if (size != command_size_flags::none
            && (command_spec.valid_sizes & size) == 0) {
                error("P011", "invalid size specifier for command");
                return nullptr;
            }

            auto command_node = std::make_shared<ast_node_t>();
            command_node->value = command_t {
                    command_spec,
                    stream.str(),
                    size};
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