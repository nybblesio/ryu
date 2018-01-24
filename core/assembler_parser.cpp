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

    static std::map<std::string, directive_t::types> s_directive_to_type = {
        // assembler control directives
        {"org",    directive_t::types::origin},
        {"target", directive_t::types::target},
        {"equ",    directive_t::types::equate},

        // data directives
        {"align",  directive_t::types::align},
        {"byte",   directive_t::types::data},
        {"word",   directive_t::types::data},
        {"dword",  directive_t::types::data},
        {"ascii",  directive_t::types::data},
        {"struct", directive_t::types::structure},

        // macros, conditionals, loops
        {"if",     directive_t::types::if_block},
        {"elseif", directive_t::types::elseif_block},
        {"else",   directive_t::types::else_block},
        {"loop",   directive_t::types::loop},
        {"macro",  directive_t::types::macro},

        // block/scope
        {"local",  directive_t::types::local},
        {"end",    directive_t::types::end_block},
    };

    assembler_parser::assembler_parser() {
    }

    ast_node_shared_ptr assembler_parser::parse(const std::string& input) {
        reset(input);
        return parse_assembly();
    }

    ast_node_shared_ptr assembler_parser::parse_assembly() {
        auto token = current_token();
        if (token == nullptr)
            return nullptr;

        auto assembly_node = std::make_shared<ast_node_t>();
        assembly_node->token = ast_node_t::tokens::assembly;

        while (true) {
            token = current_token();
            if (token == nullptr)
                break;

            consume_white_space();
            auto comment_node = parse_comment();
            if (comment_node != nullptr) {
                assembly_node->children.push_back(comment_node);
            }

            consume_white_space();
            ast_node_shared_ptr identifier_or_label_node = nullptr;
            identifier_or_label_node = parse_identifier();
            if (identifier_or_label_node == nullptr) {
                identifier_or_label_node = parse_label();
            }
            if (identifier_or_label_node != nullptr) {
                assembly_node->children.push_back(identifier_or_label_node);
            }

            consume_white_space();
            auto directive_node = parse_directive();
            if (directive_node != nullptr) {
                assembly_node->children.push_back(directive_node);
            }

            consume_white_space();
            auto expression_node = parse_expression();
            if (expression_node != nullptr) {
                assembly_node->children.push_back(expression_node);
            }
        }

        return assembly_node;
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

        auto directive_type_it = s_directive_to_type.find(directive_name);
        if (directive_type_it == s_directive_to_type.end()) {
            error("A001", "unknown assembler directive.");
            return nullptr;
        }

        auto data_size = directive_t::data_size::none;
        if (directive_type_it->second == directive_t::types::data) {
            if (directive_name == "byte") {
                data_size = directive_t::data_size::byte;
            } else if (directive_name == "word") {
                data_size = directive_t::data_size::word;
            } else if (directive_name == "dword") {
                data_size = directive_t::data_size::dword;
            } else if (directive_name == "ascii") {
                data_size = directive_t::data_size::ascii;
            }
        }

        auto directive_node = std::make_shared<ast_node_t>();
        directive_node->value = directive_t {
                directive_type_it->second,
                data_size
        };
        directive_node->token = ast_node_t::tokens::directive;

        // XXX: add a scope stack
        // directives that create a new basic_block need to push
        //   that scope onto the stack
        //

        return directive_node;
    }

}