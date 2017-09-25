//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "evaluator.h"

namespace ryu::core {

    core::symbol_table* evaluator::symbol_table() {
        return _symbol_table;
    }

    void evaluator::symbol_table(core::symbol_table* value) {
        _symbol_table = value;
    }

    variant_t evaluator::evaluate(core::result& result, const ast_node_t* node) {
        if (node == nullptr)
            return {};

        switch (node->token) {
            case ast_node_t::command:
            case ast_node_t::comment:
            case ast_node_t::null_literal:
            case ast_node_t::string_literal:
            case ast_node_t::boolean_literal:
                return node->value;
            case ast_node_t::identifier: {
                if (_symbol_table == nullptr) {
                    error(result, "E006", "no symbol table assigned");
                    break;
                }
                auto identifier = boost::get<std::string>(node->value);
                auto value = _symbol_table->get(identifier);
                if (value == nullptr) {
                    error(result, "E006", fmt::format("unknown identifier: {}", identifier));
                } else {
                    return evaluate(result, value);
                }
                break;
            }
            case ast_node_t::character_literal:
                return static_cast<uint32_t>(boost::get<char>(node->value));
            case ast_node_t::number_literal: {
                switch (node->value.which()) {
                    case 0: {
                        uint32_t out;
                        auto value = boost::get<radix_number_t>(node->value);
                        if (value.parse(out) != radix_number_t::conversion_result::success) {
                            error(result, "E004", "numeric conversion error");
                            break;
                        }
                        return out;
                    }
                    case 3: {
                        return static_cast<uint32_t>(boost::get<char>(node->value));
                    }
                }
                break;
            }
            case ast_node_t::basic_block:break;
            case ast_node_t::statement:break;
            case ast_node_t::expression: {
                return evaluate(result, node->children[0]);
            }
            case ast_node_t::binary_op: {
                auto op = boost::get<operator_t>(node->value);
                switch (op.group) {
                    case operator_t::op_group::arithmetic: {
                        auto lhs_node = evaluate(result, node->lhs);
                        if (lhs_node.which() != 1) {
                            error(result, "E005", "only integer values can be used with arithmetic operators");
                        }
                        auto rhs_node = evaluate(result, node->rhs);
                        if (rhs_node.which() != 1) {
                            error(result, "E005", "only integer values can be used with arithmetic operators");
                        }

                        if (result.is_failed())
                            break;

                        auto lhs = boost::get<uint32_t>(lhs_node);
                        auto rhs = boost::get<uint32_t>(rhs_node);
                        if (op.symbol == "+") {
                            return lhs + rhs;
                        } else if (op.symbol == "-") {
                            return lhs - rhs;
                        } else if (op.symbol == "*") {
                            return lhs * rhs;
                        } else if (op.symbol == "/") {
                            return lhs / rhs;
                        } else if (op.symbol == "\\") {
                            return lhs % rhs;
                        } else if (op.symbol == "&") {
                            return lhs & rhs;
                        } else if (op.symbol == "|") {
                            return lhs | rhs;
                        } else if (op.symbol == "^") {
                            return lhs ^ rhs;
                        }
                        break;
                    }
                    case operator_t::op_group::logical: {
                        break;
                    }
                    case operator_t::op_group::relational: {
                        break;
                    }
                    default: {
                        break;
                    }
                }
                break;
            }
            case ast_node_t::unary_op: {
                auto rhs = boost::get<uint32_t>(evaluate(result, node->rhs));
                auto op = boost::get<operator_t>(node->value);
                if (op.symbol == "-") {
                    return -rhs;
                } else if (op.symbol == "~") {
                    return ~rhs;
                }
                break;
            }
        }

        return {};
    }

    void evaluator::error(core::result& result, const std::string& code, const std::string& message) {
        result.add_message(code, message, true);
        result.fail();
    }
}