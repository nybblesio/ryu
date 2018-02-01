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
#include <boost/algorithm/string.hpp>
#include "assembler_parser.h"

namespace ryu::core {

    void assembler_parser::register_operators() {
        register_operator(
                "paste",
                {
                        operators::paste,
                        "#",
                        8,
                        operator_t::op_type::binary,
                        operator_t::associativity_type::left,
                        operator_t::op_group::conversion
                });
        register_operator(
                "quote",
                {
                        operators::quote,
                        "`",
                        11,
                        operator_t::op_type::unary,
                        operator_t::associativity_type::right,
                        operator_t::op_group::conversion
                });
        register_operator(
                "offset",
                {
                        operators::offset,
                        "offset",
                        11,
                        operator_t::op_type::unary,
                        operator_t::associativity_type::right,
                        operator_t::op_group::conversion
                });
        register_operator(
                "dup",
                {
                        operators::dup,
                        "dup",
                        8,
                        operator_t::op_type::binary,
                        operator_t::associativity_type::left,
                        operator_t::op_group::conversion
                });
    }

    ast_node_shared_ptr assembler_parser::pop_scope() {
        if (_scope_stack.empty())
            return nullptr;
        _scope_stack.pop();
        return _scope_stack.top();
    }

    void assembler_parser::parse_assembly() {
        auto token = current_token();
        if (token == nullptr)
            return;

        while (true) {
            ast_node_shared_ptr current_identifier = nullptr;
            auto scope = current_scope();

            token = current_token();
            if (token == nullptr || is_failed())
                break;

            consume_white_space();
            auto comment_node = parse_comment();
            if (comment_node != nullptr) {
                scope->children.push_back(comment_node);
            }

            consume_white_space();
            current_identifier = parse_identifier();

            consume_white_space();
            auto directive_node = parse_directive();
            if (directive_node != nullptr) {
                auto directive = boost::get<directive_t>(directive_node->value);

                if (directive.type != directive_t::types::elseif_block
                &&  directive.type != directive_t::types::else_block)
                    scope->children.push_back(directive_node);

                if (current_identifier != nullptr)
                    directive_node->lhs = current_identifier;

                consume_white_space();

                switch (directive.type) {
                    case directive_t::data: {
                        auto parameter_list_node = create_ast_node(ast_node_t::tokens::parameter_list);

                        while (true) {
                            auto expression_node = parse_expression();
                            if (expression_node != nullptr) {
                                parameter_list_node->children.push_back(expression_node);
                                consume_white_space();
                                token = current_token();
                                if (token == nullptr || *token != ',') {
                                    break;
                                }
                                move_to_next_token();
                            }
                        }

                        directive_node->rhs = parameter_list_node;
                        break;
                    }
                    case directive_t::equate: {
                        if (directive_node->lhs == nullptr
                        ||  directive_node->lhs->token != ast_node_t::tokens::identifier) {
                            error("E004", "equate directive requires a variable identifier");
                            break;
                        }
                        directive_node->rhs = parse_expression();
                        break;
                    }
                    case directive_t::align:
                    case directive_t::origin: {
                        directive_node->rhs = parse_expression();
                        break;
                    }
                    case directive_t::target: {
                        directive_node->rhs = parse_string_literal();
                        break;
                    }
                    case directive_t::local: {
                        auto statement_node = create_ast_node(ast_node_t::tokens::statement);
                        auto parameter_list_node = create_ast_node(ast_node_t::tokens::parameter_list);
                        statement_node->rhs = parameter_list_node;
                        directive_node->rhs = statement_node;

                        while (true) {
                            auto identifier_node = parse_identifier();
                            if (identifier_node != nullptr) {
                                parameter_list_node->children.push_back(identifier_node);
                                consume_white_space();
                                token = current_token();
                                if (token == nullptr || *token != ',') {
                                    break;
                                }
                                move_to_next_token();
                            }
                        }

                        break;
                    }
                    case directive_t::loop:
                    case directive_t::macro:
                    case directive_t::structure: {
                        auto parameter_list_node = create_ast_node(ast_node_t::tokens::parameter_list);

                        // XXX: need more strict parsing here
                        auto identifier_node = parse_identifier();
                        if (identifier_node != nullptr) {
                            parameter_list_node->children.push_back(identifier_node);

                            while (true) {
                                auto expression_node = parse_expression();
                                if (expression_node == nullptr)
                                    break;
                                parameter_list_node->children.push_back(expression_node);
                            }

                            auto statement_node = create_ast_node(ast_node_t::tokens::statement);
                            statement_node->lhs = parameter_list_node;
                            statement_node->rhs = create_ast_node(ast_node_t::tokens::basic_block);
                            push_scope(statement_node->rhs);

                            directive_node->rhs = statement_node;
                        } else {
                            error("A001", "missing identifier");
                        }
                        break;
                    }
                    case directive_t::end_block: {
                        pop_scope();
                        break;
                    }
                    case directive_t::if_block: {
                        directive_node->rhs = parse_expression();
                        if (directive_node->rhs == nullptr) {
                            error("A001", "missing expression");
                        } else {
                            auto branch_node = create_ast_node(ast_node_t::tokens::branch);
                            directive_node->children.push_back(branch_node);

                            auto true_basic_block = create_ast_node(ast_node_t::tokens::basic_block);
                            true_basic_block->parent = branch_node;
                            push_scope(true_basic_block);
                            branch_node->lhs = true_basic_block;
                        }
                        break;
                    }
                    case directive_t::elseif_block: {
                        directive_node->rhs = parse_expression();
                        if (directive_node->rhs == nullptr) {
                            error("A001", "missing expression");
                        } else {
                            if (scope->parent != nullptr
                            &&  scope->parent->token == ast_node_t::tokens::branch) {
                                scope->parent->rhs = directive_node;
                                scope = pop_scope();

                                auto branch_node = create_ast_node(ast_node_t::tokens::branch);
                                directive_node->children.push_back(branch_node);

                                auto true_basic_block = create_ast_node(ast_node_t::tokens::basic_block);
                                true_basic_block->parent = branch_node;
                                push_scope(true_basic_block);

                                branch_node->lhs = true_basic_block;
                            } else {
                                error("A001", "missing if expression");
                            }
                        }
                        break;
                    }
                    case directive_t::else_block:
                        if (scope->parent != nullptr
                        &&  scope->parent->token == ast_node_t::tokens::branch) {
                            scope->parent->rhs = directive_node;
                            scope = pop_scope();

                            auto true_basic_block = create_ast_node(ast_node_t::tokens::basic_block);
                            directive_node->children.push_back(true_basic_block);
                            push_scope(true_basic_block);
                        } else {
                            error("A001", "missing if expression");
                        }
                        break;
                    default:
                        break;
                }
            } else {
                if (current_identifier != nullptr)
                    scope->children.push_back(current_identifier);
            }

            // XXX: access custom assembly language parser here
        }
    }

    ast_node_shared_ptr assembler_parser::parse_directive() {
        auto token = current_token();
        if (token == nullptr)
            return nullptr;

        std::stringstream stream;
        if (*token == '.') {
            std::string s;
            while (true) {
                token = move_to_next_token();
                if (token == nullptr)
                    break;
                s = *token;
                if (std::regex_match(s, std::regex("[_a-zA-Z]")))
                    stream << *token;
                else
                    break;
            }
        }

        auto directive_name = boost::to_lower_copy<std::string>(stream.str());
        if (directive_name.empty())
            return nullptr;

        if (directive_name.length() < 2) {
            error("A001", "unexpected end of assembler directive.");
            return nullptr;
        }

        auto directive_type = directive_t::type_for_directive(directive_name);
        if (directive_type == directive_t::types::unknown) {
            error("A001", "unknown assembler directive.");
            return nullptr;
        }

        auto data_size = directive_t::size_for_directive(directive_name);
        auto directive = directive_t {directive_type, data_size};

        auto directive_node = create_ast_node(ast_node_t::tokens::directive);
        directive_node->value = directive;

        return directive_node;
    }

    ast_node_shared_ptr assembler_parser::current_scope() const {
        if (_scope_stack.empty())
            return nullptr;
        auto top = _scope_stack.top();
        return top;
    }

    ast_node_shared_ptr assembler_parser::parse(const std::string& input) {
        if (input.empty())
            return nullptr;

        auto program_node = create_ast_node(ast_node_t::tokens::program);
        push_scope(program_node);
        reset(input);
        parse_assembly();

        return current_scope();
    }

    ast_node_shared_ptr assembler_parser::push_scope(const ast_node_shared_ptr& node) {
        _scope_stack.push(node);
        return _scope_stack.top();
    }

}