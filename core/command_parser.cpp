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
    // E [name]                                 - open the text editor
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
            {"!",           {command_types::quit,                       {}}},
            {"?",           {command_types::evaluate,                   {{"...",  variant::types::variadic}}}},
            {"a",           {command_types::assemble,                   {{"addr", variant::types::numeric_literal, false}}}},
            {"e",           {command_types::text_editor,                {{"name", variant::types::string_literal, false}}}},
            {"m",           {command_types::memory_editor,              {{"addr", variant::types::numeric_literal}}}},
            {"d",           {command_types::disassemble,                {{"addr", variant::types::numeric_literal}}}},
            {"h",           {command_types::hex_dump,                   {{"addr", variant::types::numeric_literal}}}},
            {"s",           {command_types::search_memory,
                                                                        {{"addr",  variant::types::numeric_literal},
                                                                         {"bytes", variant::types::numeric_literal},
                                                                         {"value", variant::types::any}}}},
            {"f",           {command_types::fill_memory,
                                                                        {{"addr",  variant::types::numeric_literal},
                                                                         {"bytes", variant::types::numeric_literal},
                                                                         {"value", variant::types::any}}}},
            {"c",           {command_types::copy_memory,
                                                                        {{"dest",  variant::types::numeric_literal},
                                                                         {"src",   variant::types::numeric_literal},
                                                                         {"bytes", variant::types::numeric_literal}}}},
            {"j",           {command_types::jump_to_address,            {{"addr",  variant::types::numeric_literal, false}}}},
            {"g",           {command_types::go_to_address,              {{"addr",  variant::types::numeric_literal, false}}}},
            {"r",           {command_types::register_editor,            {}}},
            {"rb",          {command_types::read_binary_to_memory,
                                                                        {{"path",  variant::types::string_literal},
                                                                         {"start", variant::types::numeric_literal},
                                                                         {"end",   variant::types::numeric_literal, false}}}},
            {"wb",          {command_types::write_memory_to_binary,
                                                                        {{"path",  variant::types::string_literal},
                                                                         {"start", variant::types::numeric_literal},
                                                                         {"end",   variant::types::numeric_literal, false}}}},

            // filesystem commands
            {"rm",          {command_types::remove_file,                {{"path", variant::types::string_literal}}}},
            {"dir",         {command_types::dir,                        {}}},
            {"cd",          {command_types::change_directory,           {{"path", variant::types::string_literal}}}},
            {"pwd",         {command_types::print_working_directory,    {}}},
            {"clear",       {command_types::clear,                      {}}},

            // project commands
            {"new",         {command_types::new_project,                {{"path", variant::types::string_literal}}}},
            {"load",        {command_types::load_project,               {{"path", variant::types::string_literal}}}},
            {"save",        {command_types::save_project,               {}}},
            {"clone",       {command_types::clone_project,              {{"path", variant::types::string_literal}}}},

            // text editor commands
            {"rt",          {command_types::read_text,                  {{"path",   variant::types::string_literal}}}},
            {"wt",          {command_types::write_text,                 {{"path",   variant::types::string_literal}}}},
            {":",           {command_types::goto_line,                  {{"line",   variant::types::numeric_literal}}}},
            {"/",           {command_types::find_text,                  {{"needle", variant::types::string_literal}}}},

            // machines
            {"machines",    {command_types::machines_list,              {}}},
            {"machine",     {command_types::machine_editor,             {{"name", variant::types::string_literal}}}},
            {"delmachine",  {command_types::del_machine,                {{"name", variant::types::string_literal}}}},
            {"usemachine",  {command_types::use_machine,                {{"name", variant::types::string_literal}}}},

            // editor and tool commands
            {"%",           {command_types::open_editor,
                                                                        {{"path",  variant::types::string_literal},
                                                                         {"type",  variant::types::identifier}}}},
            {"sounds",      {command_types::sounds,                     {}}},
            {"tracker",     {command_types::tracker,                    {}}},
            {"tiles",       {command_types::tile_editor,                {}}},
            {"sprites",     {command_types::sprite_editor,              {}}},
            {"backgrounds", {command_types::background_editor,          {}}},
    };

    command_parser::command_parser() : parser() {
    }

    ast_node_t* command_parser::parse(const std::string& input) {
        reset(input);
        return parse_command();
    }

    ast_node_t* command_parser::parse_command() {
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