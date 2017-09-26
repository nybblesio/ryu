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
#include "parser.h"

namespace ryu::core {

    operator_dict parser::_operators = {
            {"~",  {operator_t::op::invert,            "~",  12, operator_t::op_type::unary,  operator_t::associativity_type::right, operator_t::op_group::arithmetic}},
            {"`",  {operator_t::op::negate,            "-",  11, operator_t::op_type::unary,  operator_t::associativity_type::right, operator_t::op_group::arithmetic}},
            {"^",  {operator_t::op::pow,               "^",  10, operator_t::op_type::binary, operator_t::associativity_type::right, operator_t::op_group::arithmetic}},
            {"*",  {operator_t::op::multiply,          "*",   9, operator_t::op_type::binary, operator_t::associativity_type::left,  operator_t::op_group::arithmetic}},
            {"/",  {operator_t::op::divide,            "/",   9, operator_t::op_type::binary, operator_t::associativity_type::left,  operator_t::op_group::arithmetic}},
            {"\\", {operator_t::op::modulo,            "\\",  9, operator_t::op_type::binary, operator_t::associativity_type::left,  operator_t::op_group::arithmetic}},
            {"+",  {operator_t::op::add,               "+",   8, operator_t::op_type::binary, operator_t::associativity_type::left,  operator_t::op_group::arithmetic}},
            {"-",  {operator_t::op::subtract,          "-",   8, operator_t::op_type::binary, operator_t::associativity_type::left,  operator_t::op_group::arithmetic}},
            {"&",  {operator_t::op::binary_and,        "&",   8, operator_t::op_type::binary, operator_t::associativity_type::left,  operator_t::op_group::arithmetic}},
            {"|",  {operator_t::op::binary_or,         "|",   8, operator_t::op_type::binary, operator_t::associativity_type::left,  operator_t::op_group::arithmetic}},
            {">",  {operator_t::op::greater_than,      ">",   7, operator_t::op_type::binary, operator_t::associativity_type::left,  operator_t::op_group::relational}},
            {">=", {operator_t::op::greater_than_equal,">=",  7, operator_t::op_type::binary, operator_t::associativity_type::left,  operator_t::op_group::relational}},
            {"<",  {operator_t::op::less_than,         "<",   7, operator_t::op_type::binary, operator_t::associativity_type::left,  operator_t::op_group::relational}},
            {"<=", {operator_t::op::less_than_equal,   "<=",  7, operator_t::op_type::binary, operator_t::associativity_type::left,  operator_t::op_group::relational}},
            {"==", {operator_t::op::equal,             "==",  6, operator_t::op_type::binary, operator_t::associativity_type::left,  operator_t::op_group::relational}},
            {"!=", {operator_t::op::not_equal,         "!=",  6, operator_t::op_type::binary, operator_t::associativity_type::left,  operator_t::op_group::relational}},
            {"&&", {operator_t::op::logical_and,       "&&",  5, operator_t::op_type::binary, operator_t::associativity_type::left,  operator_t::op_group::logical}},
            {"||", {operator_t::op::logical_or,        "||",  4, operator_t::op_type::binary, operator_t::associativity_type::left,  operator_t::op_group::logical}},
            {"(",  {operator_t::op::left_parenthesis,  "(",   0, operator_t::op_type::no_op,  operator_t::associativity_type::none,  operator_t::op_group::grouping}},
            {")",  {operator_t::op::right_parenthesis, ")",   0, operator_t::op_type::no_op,  operator_t::associativity_type::none,  operator_t::op_group::grouping}},
            {"[",  {operator_t::op::left_bracket,      "[",   0, operator_t::op_type::no_op,  operator_t::associativity_type::none,  operator_t::op_group::grouping}},
            {"]",  {operator_t::op::right_bracket,     "]",   0, operator_t::op_type::no_op,  operator_t::associativity_type::none,  operator_t::op_group::grouping}},
    };

    bool parser::has_operand() {
        return !_operand_stack.empty();
    }

    bool parser::has_operator() {
        return !_operator_stack.empty();
    }

    void parser::pop_position() {
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

    char* parser::current_token() {
        if (_token == nullptr)
            move_to_next_token();
        return _token;
    }

    void parser::push_position() {
        _position_stack.push(scanner_pos_t {
                _line,
                std::min(_index, static_cast<int>(_input.length())),
                _column});
    }

    void parser::increment_line() {
        _column = 1;
        _line++;
    }

    ast_node_t* parser::pop_operand() {
        if (_operand_stack.empty())
            return nullptr;
        auto top = _operand_stack.top();
        _operand_stack.pop();
        return top;
    }

    void parser::forget_top_position() {
        if (_position_stack.empty())
            return;
        _position_stack.pop();
    }

    char* parser::move_to_next_token() {
        _index++;
        if (_index > _input.length() - 1) {
            _token = nullptr;
        } else {
            _column++;
            _token = &_input[_index];
        }
        return _token;
    }

    void parser::consume_white_space() {
        auto token = current_token();
        while (token != nullptr && isspace(*token)) {
            if (*token == '\n')
                increment_line();
            token = move_to_next_token();
            if (token == nullptr)
                return;
        }
    }

    ast_node_t* parser::parse_number() {
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

    operator_t* parser::pop_operator() {
        if (_operator_stack.empty())
            return nullptr;
        auto top = _operator_stack.top();
        _operator_stack.pop();
        return top;
    }

    operator_t* parser::peek_operator() {
        if (_operator_stack.empty())
            return nullptr;
        return _operator_stack.top();
    }

    void parser::clear_position_stack() {
        while (!_position_stack.empty())
            _position_stack.pop();
    }

    ast_node_t* parser::parse_comment() {
        auto token = current_token();
        if (token == nullptr)
            return nullptr;
        if (*token == '*' || *token == ';') {
            token = move_to_next_token();
            std::stringstream stream;
            while (token != nullptr && *token != '\n') {
                stream << *token;
                token = move_to_next_token();
            }
            auto comment = new ast_node_t();
            comment->value = stream.str();
            comment->token = ast_node_t::tokens::comment;
            return comment;
        }
        return nullptr;
    }

    operator_t* parser::parse_operator() {
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

    ast_node_t* parser::parse_identifier() {
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

    symbol_table* parser::symbol_table() {
        return _symbol_table;
    }

    ast_node_t* parser::parse_expression() {
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
                while (has_operator()) {
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
                    while (has_operator()) {
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
                    const std::vector<std::function<ast_node_t* ()>> terminals = {
                            [&] () {return parse_number();},
                            [&] () {return parse_comment();},
                            [&] () {return parse_identifier();},
                            [&] () {return parse_null_literal();},
                            [&] () {return parse_string_literal();},
                            [&] () {return parse_boolean_literal();},
                            [&] () {return parse_character_literal();},
                    };
                    for (const auto& terminal : terminals) {
                        auto node = terminal();
                        if (node != nullptr) {
                            push_operand(node);
                            break;
                        }
                    }
                }
            }
        }

        while (has_operator()) {
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

    void parser::consume_tokens(int count) {
        for (auto i = 0; i < count; ++i) {
            auto token = move_to_next_token();
            if (token == nullptr)
                break;
        }
    }

    ast_node_t* parser::parse_null_literal() {
        auto token = current_token();
        if (token == nullptr)
            return nullptr;
        if (std::regex_match(token, std::regex("[nN][uU][lL][lL]"))) {
            consume_tokens(4);
            auto identifier_node = new ast_node_t();
            identifier_node->token = ast_node_t::tokens::null_literal;
            return identifier_node;
        }
        return nullptr;
    }

    void parser::push_operator(operator_t* op) {
        _operator_stack.push(op);
    }

    ast_node_t* parser::parse_string_literal() {
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

    const core::result& parser::result() const {
        return _result;
    }

    void parser::push_operand(ast_node_t* node) {
        _operand_stack.push(node);
    }

    ast_node_t* parser::parse_boolean_literal() {
        auto token = current_token();
        if (token == nullptr)
            return nullptr;
        if (std::regex_match(token, std::regex("[tT][rR][uU][eE]"))) {
            consume_tokens(4);
            auto identifier_node = new ast_node_t();
            identifier_node->value = true;
            identifier_node->token = ast_node_t::tokens::boolean_literal;
            return identifier_node;
        } else if (std::regex_match(token, std::regex("[fF][aA][lL][sS][eE]"))) {
            consume_tokens(5);
            auto identifier_node = new ast_node_t();
            identifier_node->value = false;
            identifier_node->token = ast_node_t::tokens::boolean_literal;
            return identifier_node;
        }
        return nullptr;
    }

    void parser::reset(const std::string& input) {
        _line = 1;
        _column = 1;
        _index = -1;
        _result = {};
        _input = input;
        clear_position_stack();
    }

    ast_node_t* parser::parse_character_literal() {
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

    void parser::symbol_table(core::symbol_table* value) {
        _symbol_table = value;
    }

    ast_node_t* parser::parse_expression(const std::string& input) {
        reset(input);
        return parse_expression();
    }

    void parser::error(const std::string& code, const std::string& message) {
        std::stringstream stream;
        stream << "Syntax error: " << message << "\n"
               << _input << "\n"
               << std::setw(_column) << '^';
        _result.add_message(code, stream.str(), true);
        _result.fail();
    }

}