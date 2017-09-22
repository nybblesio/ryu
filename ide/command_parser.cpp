//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <regex>
#include <iomanip>
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
            {"~",  {"~",  12, operator_t::op_type::unary,  operator_t::associativity_type::right}},
            {"`",  {"-",  11, operator_t::op_type::unary,  operator_t::associativity_type::right}},
            {"^",  {"^",  10, operator_t::op_type::binary, operator_t::associativity_type::right}},
            {"*",  {"*",   9, operator_t::op_type::binary, operator_t::associativity_type::left}},
            {"/",  {"/",   9, operator_t::op_type::binary, operator_t::associativity_type::left}},
            {"%",  {"%",   9, operator_t::op_type::binary, operator_t::associativity_type::left}},
            {"+",  {"+",   8, operator_t::op_type::binary, operator_t::associativity_type::left}},
            {"-",  {"-",   8, operator_t::op_type::binary, operator_t::associativity_type::left}},
            {">",  {">",   7, operator_t::op_type::binary, operator_t::associativity_type::left}},
            {">=", {">=",  7, operator_t::op_type::binary, operator_t::associativity_type::left}},
            {"<",  {"<",   7, operator_t::op_type::binary, operator_t::associativity_type::left}},
            {"<=", {"<=",  7, operator_t::op_type::binary, operator_t::associativity_type::left}},
            {"==", {"==",  6, operator_t::op_type::binary, operator_t::associativity_type::left}},
            {"!=", {"!=",  6, operator_t::op_type::binary, operator_t::associativity_type::left}},
            {"&&", {"&&",  5, operator_t::op_type::binary, operator_t::associativity_type::left}},
            {"||", {"||",  4, operator_t::op_type::binary, operator_t::associativity_type::left}},
            {"(",  {"(",   0, operator_t::op_type::no_op,  operator_t::associativity_type::none}},
            {")",  {")",   0, operator_t::op_type::no_op,  operator_t::associativity_type::none}},
            {"[",  {"[",   0, operator_t::op_type::no_op,  operator_t::associativity_type::none}},
            {"]",  {"]",   0, operator_t::op_type::no_op,  operator_t::associativity_type::none}},
    };

    command_parser::command_parser(const std::string& input) : _input(input) {
    }

    ast_node_t* command_parser::parse() {
        reset_position();
        return parse_command();
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

    char* command_parser::current_token() {
        if (_token == nullptr)
            move_to_next_token();
        return _token;
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

    ast_node_t* command_parser::pop_operand() {
        if (_operand_stack.empty())
            return nullptr;
        auto top = _operand_stack.top();
        _operand_stack.pop();
        return top;
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
        auto token = current_token();
        while (token != nullptr && isspace(*token)) {
            if (*token == '\n')
                increment_line();
            token = move_to_next_token();
            if (token == nullptr)
                return;
        }
    }

    ast_node_t* command_parser::parse_number() {
        radix_number_t number;

        auto token = current_token();
        if (token == nullptr)
            return nullptr;

        if (*token == '$') {
            std::stringstream stream;
            while (true) {
                token = move_to_next_token();
                if (token == nullptr)
                    break;
                if (!ishexnumber(*token))
                    break;
                stream << *token;
            }
            auto value = stream.str();
            if (!value.empty()) {
                number.radix = 16;
                number.value = value;
            }
        } else if (*token == '@') {
            const std::string valid = "012345678";
            std::stringstream stream;
            while (true) {
                token = move_to_next_token();
                if (token == nullptr)
                    break;
                if (valid.find_first_of(*token) == std::string::npos)
                    break;
                stream << *token;
            }
            auto value = stream.str();
            if (!value.empty()) {
                number.radix = 8;
                number.value = value;
            }
        } else if (*token == '%') {
            std::stringstream stream;
            while (true) {
                token = move_to_next_token();
                if (token == nullptr)
                    break;
                if (*token != '0' && *token != '1')
                    break;
                stream << *token;
            }
            auto value = stream.str();
            if (!value.empty()) {
                number.radix = 2;
                number.value = value;
            }
        } else {
            const std::string valid = "0123456789";
            std::stringstream stream;
            while (valid.find_first_of(*token) != std::string::npos) {
                stream << *token;
                token = move_to_next_token();
                if (token == nullptr)
                    break;
            }
            auto value = stream.str();
            if (!value.empty()) {
                number.radix = 10;
                number.value = value;
            }
        }

        if (number.radix != 0) {
            auto number_literal = new ast_node_t();
            number_literal->value = number;
            number_literal->token = ast_node_t::tokens::number_literal;
            return number_literal;
        }

        return nullptr;
    }

    operator_t* command_parser::pop_operator() {
        if (_operator_stack.empty())
            return nullptr;
        auto top = _operator_stack.top();
        _operator_stack.pop();
        return top;
    }

    operator_t* command_parser::peek_operator() {
        if (_operator_stack.empty())
            return nullptr;
        return _operator_stack.top();
    }

    void command_parser::clear_position_stack() {
        while (!_position_stack.empty())
            _position_stack.pop();
    }

    ast_node_t* command_parser::parse_command() {
        auto token = current_token();
        std::stringstream stream;
        if (!isspace(*token)) {
            stream << *token;
            while (true) {
                token = move_to_next_token();
                if (token == nullptr)
                    break;
                if (!isspace(*token))
                    stream << *token;
                else
                    break;
            }
            auto it = _commands.find(stream.str());
            if (it == _commands.end()) {
                error("P010", "unknown command");
                return nullptr;
            }
            auto command_node = new ast_node_t();
            command_node->value = command_t {it->second, stream.str()};
            command_node->token = ast_node_t::tokens::command;

            while (true) {
                auto expr = parse_expression();
                if (expr == nullptr)
                    break;
                command_node->children.push_back(expr);
            }

            return command_node;
        }
        return nullptr;
    }

    operator_t* command_parser::parse_operator() {
        push_position();

        for (auto it = _operators.begin(); it != _operators.end(); ++it) {
            if (it->second.precedence == 0)
                continue;

            auto token = current_token();
            if (*token == it->second.symbol[0]) {
                token = move_to_next_token();
                if (token == nullptr)
                    break;

                if (it->second.symbol.length() > 1) {
                    if (*token == it->second.symbol[1]) {
                        token = move_to_next_token();
                    } else {
                        error("P009", "unexpected operator");
                        pop_position();
                        return nullptr;
                    }
                }

                forget_top_position();
                auto op = &it->second;

                if (_last_operator == nullptr || _last_operator->symbol != ")") {
                    if (op->symbol == "-")
                        op = &_operators["`"];
                    else if (op->symbol == "(") {
                        error("C008", "unexpected binary operator");
                    }
                }

                _last_operator = op;
                return op;
            }
        }

        pop_position();
        return nullptr;
    }

    ast_node_t* command_parser::parse_identifier() {
        auto token = current_token();
        std::string s;
        s = *token;
        if (std::regex_match(s, std::regex("[_a-zA-Z]"))) {
            std::stringstream stream;
            stream << *token;
            while (true) {
                token = move_to_next_token();
                if (token == nullptr)
                    break;
                s = *token;
                if (std::regex_match(s, std::regex("[_a-zA-Z0-9]")))
                    stream << *token;
                else
                    break;
            }
            auto identifier_node = new ast_node_t();
            identifier_node->value = stream.str();
            identifier_node->token = ast_node_t::tokens::identifier;
            return identifier_node;
        }
        return nullptr;
    }

    ast_node_t* command_parser::parse_expression() {
        while (true) {
        main:
            consume_white_space();

            auto token = current_token();
            if (token == nullptr) {
                break;
            }

            if (*token == '(') {
                push_operator(&_operators["("]);
                _last_operator = &_operators["("];
                token = move_to_next_token();
            } else if (*token == ')') {
                _last_operator = &_operators[")"];
                token = move_to_next_token();
                while (!_operator_stack.empty()) {
                    auto op = pop_operator();
                    if (op == &_operators["("]) {
                        auto subexpression = new ast_node_t();
                        subexpression->token = ast_node_t::tokens::expression;
                        subexpression->children.push_back(pop_operand());
                        push_operand(subexpression);
                        goto main;
                    }
                    auto bin_op_node = new ast_node_t();
                    bin_op_node->value = *op;
                    bin_op_node->rhs = pop_operand();
                    bin_op_node->lhs = pop_operand();
                    bin_op_node->token = ast_node_t::tokens::binary_op;
                    push_operand(bin_op_node);
                }
                error("P008", "unbalanced right parentheses");
            } else {
                auto op = parse_operator();
                if (op != nullptr) {
                    while (!_operator_stack.empty()) {
                        auto top = peek_operator();
                        if ((op->not_right_associative() && op->compare_precedence(*top) == 0)
                        ||  (op->compare_precedence(*top) < 0)) {
                            pop_operator();
                            if ((top->type & operator_t::op_type::unary) != 0) {
                                auto unary_op_node = new ast_node_t();
                                unary_op_node->value = *top;
                                unary_op_node->rhs = pop_operand();
                                unary_op_node->token = ast_node_t::tokens::unary_op;
                                push_operand(unary_op_node);
                            } else {
                                auto bin_op_node = new ast_node_t();
                                bin_op_node->value = *top;
                                bin_op_node->rhs = pop_operand();
                                bin_op_node->lhs = pop_operand();
                                bin_op_node->token = ast_node_t::tokens::binary_op;
                                push_operand(bin_op_node);
                            }
                        } else
                            break;
                    }
                    push_operator(op);
                } else {
                    auto number = parse_number();
                    if (number != nullptr) {
                        push_operand(number);
                    } else {
                        auto identifier = parse_identifier();
                        if (identifier != nullptr) {
                            push_operand(identifier);
                        } else {
                            auto string_lit = parse_literal_string();
                            if (string_lit != nullptr) {
                                push_operand(string_lit);
                            } else {
                                auto character_lit = parse_literal_character();
                                if (character_lit != nullptr) {
                                    push_operand(character_lit);
                                } else
                                    break;
                            }
                        }
                    }
                }
            }
        }

        while (!_operator_stack.empty()) {
            auto op = pop_operator();

            if (op == &_operators["("]) {
                error("P008", "unbalanced left parentheses");
            } else if (op == &_operators[")"]) {
                error("P008", "unbalanced right parentheses");
            }

            if ((op->type & operator_t::op_type::unary) != 0) {
                auto unary_op_node = new ast_node_t();
                unary_op_node->value = *op;
                unary_op_node->rhs = pop_operand();
                unary_op_node->token = ast_node_t::tokens::unary_op;
                push_operand(unary_op_node);
            }
            if ((op->type & operator_t::op_type::binary) != 0) {
                auto bin_op_node = new ast_node_t();
                bin_op_node->value = *op;
                bin_op_node->rhs = pop_operand();
                bin_op_node->lhs = pop_operand();
                bin_op_node->token = ast_node_t::tokens::binary_op;
                push_operand(bin_op_node);
            }
        }

        return pop_operand();
    }

    void command_parser::push_operator(operator_t* op) {
        _operator_stack.push(op);
    }

    ast_node_t* command_parser::parse_literal_string() {
        auto token = current_token();
        std::stringstream stream;
        if (*token == '\"') {
            while (true) {
                token = move_to_next_token();
                if (token == nullptr || *token == '\"') {
                    break;
                } else if (*token =='\\') {
                    token = move_to_next_token();
                }
                stream << *token;
            }
            auto value = stream.str();
            if (!value.empty()) {
                auto string_literal = new ast_node_t();
                string_literal->value = value;
                string_literal->token = ast_node_t::tokens::string_literal;
                return string_literal;
            }
        }
        return nullptr;
    }

    const core::result& command_parser::result() const {
        return _result;
    }

    void command_parser::push_operand(ast_node_t* node) {
        _operand_stack.push(node);
    }

    ast_node_t* command_parser::parse_literal_character() {
        auto token = current_token();
        if (token != nullptr && *token == '\'') {
            char value;
            token = move_to_next_token();
            if (token != nullptr) {
                value = *token;
                token = move_to_next_token();
                if (token != nullptr && *token == '\'') {
                    move_to_next_token();
                    auto character_literal = new ast_node_t();
                    character_literal->value = value;
                    character_literal->token = ast_node_t::tokens::character_literal;
                    return character_literal;
                } else {
                    error("C008", "unbalanced single quote of character literal");
                }
            } else {
                error("C010", "unexpected end of input");
            }
        }
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

    void command_parser::error(const std::string& code, const std::string& message) {
        std::stringstream stream;
        stream << "Syntax error: " << message << "\n"
               << _input << "\n"
               << std::setw(_column) << '^';
        _result.add_message(code, stream.str(), true);
        _result.fail();
    }

}