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

namespace ryu::ide {

    // commands are not case sensitive
    //
    // !                                        - quit
    //
    // A                                        - assemble
    // A [addr]                                 - assemble
    //
    // ?                                        - expression evaluator
    // ? 'A'                                    - convert character to ASCII value
    // ? [number]                               - display various base values of number
    // ? [expression]                           - evaluate arithmetic expression
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

    operator_dict command_parser::_operators = {
            {"~",  {operator_t::op_type::unary,  "~",  11, operator_t::associativity_type::right}},
            {"^",  {operator_t::op_type::binary, "^",  10, operator_t::associativity_type::right}},
            {"*",  {operator_t::op_type::binary, "*",   9, operator_t::associativity_type::left}},
            {"/",  {operator_t::op_type::binary, "/",   9, operator_t::associativity_type::left}},
            {"%",  {operator_t::op_type::binary, "%",   9, operator_t::associativity_type::left}},
            {"+",  {operator_t::op_type::any,    "+",   8, operator_t::associativity_type::left}},
            {"-",  {operator_t::op_type::any,    "-",   8, operator_t::associativity_type::left}},
            {">",  {operator_t::op_type::binary, ">",   7, operator_t::associativity_type::left}},
            {">=", {operator_t::op_type::binary, ">=",  7, operator_t::associativity_type::left}},
            {"<",  {operator_t::op_type::binary, "<",   7, operator_t::associativity_type::left}},
            {"<=", {operator_t::op_type::binary, "<=",  7, operator_t::associativity_type::left}},
            {"==", {operator_t::op_type::binary, "==",  6, operator_t::associativity_type::left}},
            {"!=", {operator_t::op_type::binary, "!=",  6, operator_t::associativity_type::left}},
            {"&&", {operator_t::op_type::binary, "&&",  5, operator_t::associativity_type::left}},
            {"||", {operator_t::op_type::binary, "||",  4, operator_t::associativity_type::left}},
            {"(",  {operator_t::op_type::any,    "(",   1, operator_t::associativity_type::none}},
            {")",  {operator_t::op_type::any,    ")",   1, operator_t::associativity_type::none}},
            {"[",  {operator_t::op_type::any,    "[",   0, operator_t::associativity_type::none}},
            {"]",  {operator_t::op_type::any,    "]",   0, operator_t::associativity_type::none}},
    };

    command_parser::command_parser(const std::string& input) : _input(input) {
    }

    ast_node_t command_parser::parse() {
        ast_node_t root {};
        reset_position();
        return root;
    }

    void command_parser::pop_position() {
        if (_position_stack.empty())
            return;

        auto pos = _position_stack.top();
        _line = pos.line;
        _index = pos.index;
        _column = pos.column;
        if (_index < _input.length())
            _token = &_input[_index];
        else
            _token = nullptr;
        _position_stack.pop();
    }

    void command_parser::push_position() {
        _position_stack.push(scanner_pos_t {
                _line,
                std::min(_index, static_cast<int>(_input.length())),
                _column});
    }

    void command_parser::reset_position() {
        _line = 1;
        _column = 1;
        _index = -1;
        clear_position_stack();
    }

    void command_parser::increment_line() {
        _column = 1;
        _line++;
    }

    void command_parser::forget_top_position() {
        if (_position_stack.empty())
            return;
        _position_stack.pop();
    }

    char* command_parser::move_to_next_token() {
        _index++;
        if (_index > _input.length() - 1) {
            _token = nullptr;
        } else {
            _column++;
            _token = &_input[_index];
        }
        return _token;
    }

    void command_parser::consume_white_space() {
        while (true) {
            auto token = move_to_next_token();
            if (isspace(*token))
                break;
        }
    }

    ast_node_t* command_parser::parse_number() {
        return nullptr;
    }

    void command_parser::clear_position_stack() {
        while (!_position_stack.empty())
            _position_stack.pop();
    }

    ast_node_t* command_parser::parse_command() {
        return nullptr;
    }

    ast_node_t* command_parser::parse_identifier() {
        return nullptr;
    }

    ast_node_t* command_parser::parse_expression() {
        return nullptr;
    }

    ast_node_t* command_parser::parse_literal_string() {
        return nullptr;
    }

    ast_node_t* command_parser::parse_literal_character() {
        return nullptr;
    }

    const variant_t* command_parser::symbol(const std::string& name) const {
        auto it = _symbols.find(name);
        if (it == _symbols.end())
            return nullptr;
        return &(it->second);
    }

    void command_parser::symbol(const std::string& name, const variant_t& value) {
        _symbols[name] = value;
    }

}