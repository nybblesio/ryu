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

    // commands are not case sensitive
    //
    // !                                        - quit
    //
    // A                                        - assemble
    // A [addr]                                 - assemble
    //
    // ?.[size] [expression] ...                - expression evaluator, where [size] can be .b .w .dw
    //
    // E                                        - open the text editor
    // M [addr]                                 - open hex editor
    // D [addr]                                 - disassemble
    // H [addr]                                 - hex dump into console
    // S [addr] [bytes] [value]                 - search memory
    // F [addr] [bytes] [value]                 - fill memory
    // C [dst addr] [src addr] [bytes]          - copy memory
    //
    // J [addr]                                 - jump to address and begin execution
    // G [addr]                                 - go to address
    // R                                        - view/edit CPU registers
    //
    // CLEAR                                    - clear the console screen and move caret to home position
    // CD [path]                                - change directory
    // PWD                                      - print current directory
    // NEW [filename]                           - create a new project
    // LOAD [filename]                          - load an existing project from disk
    // SAVE                                     - save the open project to disk
    // CLONE [filename]                         - saves open project to a new file
    // RM [filename]                            - delete filename
    // DIR                                      - list contents of the current directory
    //
    // MACHINES                                 - list the machines available in the registry
    // MACHINE [name]                           - open editor for machine name or create a new machine
    // DELMACHINE [name]                        - remove the named machine from the registry
    // USEMACHINE [name]                        - set the machine as current for the project
    //
    // RB [filename] [start addr] [end addr]    - read binary into memory
    // WB [filename] [start addr] [end addr]    - write memory to binary file on disk
    //
    // RT [filename]                            - read text file into editor buffer
    // WT [filename]                            - write text in buffer to file
    // :  [number]                              - goto line number
    // /  [needle]                              - find needle in buffer
    //
    command_table command_parser::_commands = {
            // assembler & memory commands
            {"!",           command_t::types::quit},
            {"?",           command_t::types::evaluate},
            {"a",           command_t::types::assemble},
            {"e",           command_t::types::text_editor},
            {"m",           command_t::types::memory_editor},
            {"d",           command_t::types::disassemble},
            {"h",           command_t::types::hex_dump},
            {"s",           command_t::types::search_memory},
            {"f",           command_t::types::fill_memory},
            {"c",           command_t::types::copy_memory},
            {"j",           command_t::types::jump_to_address},
            {"g",           command_t::types::go_to_address},
            {"r",           command_t::types::register_editor},
            {"rb",          command_t::types::read_binary_to_memory},
            {"wb",          command_t::types::write_memory_to_binary},

            // filesystem commands
            {"rm",          command_t::types::remove_file},
            {"dir",         command_t::types::dir},
            {"cd",          command_t::types::change_directory},
            {"pwd",         command_t::types::print_working_directory},
            {"clear",       command_t::types::clear},

            // project commands
            {"new",         command_t::types::new_project},
            {"load",        command_t::types::load_project},
            {"save",        command_t::types::save_project},
            {"clone",       command_t::types::clone_project},

            // text editor commands
            {"rt",          command_t::types::read_text},
            {"wt",          command_t::types::write_text},
            {":",           command_t::types::goto_line},
            {"/",           command_t::types::find_text},

            // machines
            {"machines",    command_t::types::machines_list},
            {"machine",     command_t::types::machine_editor},
            {"delmachine",  command_t::types::del_machine},
            {"usemachine",  command_t::types::use_machine},

            // editor and tool commands
            {"%",           command_t::types::open_editor},
            {"sounds",      command_t::types::sounds},
            {"tracker",     command_t::types::tracker},
            {"tiles",       command_t::types::tile_editor},
            {"sprites",     command_t::types::sprite_editor},
            {"backgrounds", command_t::types::background_editor},
    };

    command_parser::command_parser() : parser() {
    }

    ast_node_t* command_parser::parse(const std::string& input) {
        reset(input);
        return parse_command();
    }

    ast_node_t* command_parser::parse_command() {
        auto token = current_token();
        std::stringstream stream;
        if (!isspace(*token)) {
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
            auto command_node = new ast_node_t();
            command_node->value = command_t {it->second, stream.str(), size};
            command_node->token = ast_node_t::tokens::command;

            while (true) {
                auto expr = parse_expression();
                if (expr == nullptr)
                    break;
                command_node->children.insert(command_node->children.begin(), expr);
            }

            return command_node;
        }
        return nullptr;
    }

}