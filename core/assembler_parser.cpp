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

    void assembler_parser::pop_scope() {
        if (_scope_stack.empty())
            return;
        _scope_stack.pop();
    }

    ast_node_shared_ptr assembler_parser::parse_assembly() {
        auto token = current_token();
        if (token == nullptr)
            return nullptr;

        auto scope = current_scope();

        while (true) {
            token = current_token();
            if (token == nullptr)
                break;

            consume_white_space();
            auto comment_node = parse_comment();
            if (comment_node != nullptr) {
                scope->children.push_back(comment_node);
            }

            consume_white_space();
            ast_node_shared_ptr identifier_or_label_node = nullptr;
            identifier_or_label_node = parse_identifier();
            if (identifier_or_label_node == nullptr) {
                identifier_or_label_node = parse_label();
            }
            if (identifier_or_label_node != nullptr) {
                scope->children.push_back(identifier_or_label_node);
            }

            consume_white_space();
            auto directive_node = parse_directive();
            if (directive_node != nullptr) {
                scope->children.push_back(directive_node);

                consume_white_space();

                auto directive = boost::get<directive_t>(directive_node->value);
                switch (directive.type) {
                    case directive_t::data: {
                        while (true) {
                            auto expression_node = parse_expression();
                            if (expression_node != nullptr) {
                                directive_node->children.push_back(expression_node);
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
                    case directive_t::align:
                    case directive_t::equate:
                    case directive_t::origin: {
                        auto expression_node = parse_expression();
                        if (expression_node != nullptr) {
                            directive_node->children.push_back(expression_node);
                        }
                        break;
                    }
                    case directive_t::target: {
                        auto string_node = parse_string_literal();
                        if (string_node != nullptr) {
                            directive_node->children.push_back(string_node);
                        }
                        break;
                    }
                    case directive_t::loop:
                    case directive_t::local:
                    case directive_t::macro:
                    case directive_t::structure: {
                        auto identifier_node = parse_identifier();
                        if (identifier_node != nullptr) {
                            directive_node->children.push_back(identifier_node);
                        }
                        break;
                    }
                    case directive_t::if_block:
                    case directive_t::else_block:
                    case directive_t::elseif_block: {
                        token = current_token();
                        if (token != nullptr && *token == '(') {
                            move_to_next_token();
                            auto expression_node = parse_expression();
                            if (expression_node != nullptr) {
                                token = move_to_next_token();
                                consume_white_space();
                                if (token != nullptr && *token == ')') {
                                    directive_node->children.push_back(expression_node);
                                }
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        return scope;
    }

    ast_node_shared_ptr assembler_parser::parse_label() {
        auto ast_node = parse_identifier();
        auto token = current_token();
        if (ast_node != nullptr && token != nullptr && *token == ':') {
            auto identifier = boost::get<identifier_t>(ast_node->value);
            ast_node->value = label_t {identifier.value};
            ast_node->token = ast_node_t::tokens::label;
        }
        return nullptr;
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

        if (directive.is_block_end()) {
            pop_scope();
            return nullptr;
        }

        auto directive_node = std::make_shared<ast_node_t>();
        directive_node->value = directive;
        directive_node->token = ast_node_t::tokens::directive;

        if (directive.is_block()) {
            auto basic_block = std::make_shared<ast_node_t>();
            basic_block->token = ast_node_t::tokens::basic_block;
            directive_node->children.push_back(basic_block);
            push_scope(basic_block);
        }

        return directive_node;
    }

    ast_node_shared_ptr assembler_parser::current_scope() const {
        if (_scope_stack.empty())
            return nullptr;
        return _scope_stack.top();
    }

    void assembler_parser::push_scope(const ast_node_shared_ptr& node) {
        _scope_stack.push(node);
    }

    ast_node_shared_ptr assembler_parser::parse(const std::string& input) {
        auto program_node = std::make_shared<ast_node_t>();
        program_node->token = ast_node_t::tokens::program;

        push_scope(program_node);

        reset(input);

        return parse_assembly();
    }

}