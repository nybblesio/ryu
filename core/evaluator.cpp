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
            case ast_node_t::character_literal:
                return node->value;
            case ast_node_t::identifier: {
                if (_symbol_table == nullptr) {
                    error(result, "E006", "no symbol table assigned");
                    break;
                }
                auto identifier = boost::get<identifier_t>(node->value);
                auto value = _symbol_table->get(identifier.value);
                if (value == nullptr) {
                    error(result, "E006", fmt::format("unknown identifier: {}", identifier.value));
                } else {
                    return evaluate(result, value);
                }
                break;
            }
            case ast_node_t::number_literal: {
                switch (node->value.which()) {
                    case variant::types::radix_numeric_literal: {
                        int32_t out;
                        auto value = boost::get<radix_number_t>(node->value);
                        if (value.parse(out) != radix_number_t::conversion_result::success) {
                            error(result, "E004", "numeric conversion error");
                            break;
                        }
                        return numeric_literal_t{out};
                    }
                    case variant::types::char_literal: {
                        return numeric_literal_t {boost::get<char_literal_t>(node->value).value};
                    }
                    case variant::types::numeric_literal: {
                        return node->value;

                    }
                    default: {
                        error(result, "E004", "numeric conversion error");
                        break;
                    }
                }
                break;
            }
            case ast_node_t::basic_block: {
                break;
            }
            case ast_node_t::statement: {
                break;
            }
            case ast_node_t::expression: {
                return evaluate(result, node->children[0]);
            }
            case ast_node_t::binary_op: {
                auto op = boost::get<operator_t>(node->value);
                switch (op.group) {
                    case operator_t::op_group::arithmetic: {
                        auto lhs_node = evaluate(result, node->lhs);
                        if (lhs_node.which() != variant::types::numeric_literal) {
                            error(result, "E005", "only integer values can be used with arithmetic operators");
                        }
                        auto rhs_node = evaluate(result, node->rhs);
                        if (rhs_node.which() != variant::types::numeric_literal) {
                            error(result, "E005", "only integer values can be used with arithmetic operators");
                        }

                        if (result.is_failed())
                            break;

                        auto lhs = boost::get<numeric_literal_t>(lhs_node);
                        auto rhs = boost::get<numeric_literal_t>(rhs_node);
                        switch (op.op) {
                            case operator_t::op::add:        return lhs + rhs;
                            case operator_t::op::subtract:   return lhs - rhs;
                            case operator_t::op::multiply:   return lhs * rhs;
                            case operator_t::op::divide:     return lhs / rhs;
                            case operator_t::op::modulo:     return lhs % rhs;
                            case operator_t::op::binary_and: return lhs & rhs;
                            case operator_t::op::binary_or:  return lhs | rhs;
                            case operator_t::op::pow:        return lhs ^ rhs;
                            default:
                                error(result, "E008", fmt::format("operator {} is not supported", op.symbol));
                                break;
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
                auto rhs_node = evaluate(result, node->rhs);
                if (rhs_node.which() != variant::types::numeric_literal) {
                    error(result, "E005", "only integer values can be used with arithmetic operators");
                    break;
                }
                auto rhs = boost::get<numeric_literal_t>(rhs_node);
                auto op = boost::get<operator_t>(node->value);
                switch (op.op) {
                    case operator_t::op::negate: return -rhs;
                    case operator_t::op::invert: return ~rhs;
                    default:
                        error(result, "E008", fmt::format("operator {} is not supported", op.symbol));
                        break;
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