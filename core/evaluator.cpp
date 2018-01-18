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

    void evaluator::error(
            core::result& result,
            const std::string& code,
            const std::string& message) {
        result.add_message(code, message, true);
        result.fail();
    }

    variant_t evaluator::evaluate(
            core::result& result,
            const ast_node_shared_ptr& node) {
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
                if (value == nullptr && _symbol_table->missing_is_error()) {
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
                auto lhs_node = evaluate(result, node->lhs);
                auto rhs_node = evaluate(result, node->rhs);
                switch (op.group) {
                    case operator_t::op_group::relational:
                    case operator_t::op_group::arithmetic: {
                        numeric_literal_t lhs;
                        numeric_literal_t rhs;
                        if (lhs_node.which() == variant::types::boolean_literal) {
                            lhs = numeric_literal_t {boost::get<boolean_literal_t>(lhs_node).value};
                        } else if (lhs_node.which() == variant::types::numeric_literal) {
                            lhs = boost::get<numeric_literal_t>(lhs_node);
                        } else {
                            error(result,
                                  "E005",
                                  "only integer values can be used with arithmetic/relational operators");
                        }
                        if (rhs_node.which() == variant::types::boolean_literal) {
                            rhs = numeric_literal_t {boost::get<boolean_literal_t>(rhs_node).value};
                        } else if (rhs_node.which() == variant::types::numeric_literal) {
                            rhs = boost::get<numeric_literal_t>(rhs_node);
                        } else {
                            error(result,
                                  "E005",
                                  "only integer values can be used with arithmetic/relational operators");
                        }

                        if (result.is_failed())
                            break;

                        switch (op.op) {
                            case operator_t::op::add:                   return lhs + rhs;
                            case operator_t::op::subtract:              return lhs - rhs;
                            case operator_t::op::multiply:              return lhs * rhs;
                            case operator_t::op::divide:                return lhs / rhs;
                            case operator_t::op::modulo:                return lhs % rhs;
                            case operator_t::op::binary_and:            return lhs & rhs;
                            case operator_t::op::binary_or:             return lhs | rhs;
                            case operator_t::op::pow:                   return lhs ^ rhs;
                            case operator_t::op::less_than:             return lhs < rhs;
                            case operator_t::op::less_than_equal:       return lhs <= rhs;
                            case operator_t::op::equal:                 return lhs == rhs;
                            case operator_t::op::not_equal:             return lhs != rhs;
                            case operator_t::op::greater_than:          return lhs > rhs;
                            case operator_t::op::greater_than_equal:    return lhs >= rhs;
                            default:
                                error(result, "E008", fmt::format("operator {} is not supported", op.symbol));
                                break;
                        }
                        break;
                    }
                    case operator_t::op_group::logical: {
                        boolean_literal_t lhs;
                        boolean_literal_t rhs;
                        if (lhs_node.which() == variant::types::numeric_literal) {
                            lhs = boolean_literal_t {boost::get<numeric_literal_t>(lhs_node).value == 1};
                        } else if (lhs_node.which() == variant::types::boolean_literal) {
                            lhs = boost::get<boolean_literal_t>(lhs_node);
                        } else {
                            error(result,
                                  "E005",
                                  "only boolean values can be used with logical operators");
                        }
                        if (rhs_node.which() == variant::types::numeric_literal) {
                            rhs = boolean_literal_t {boost::get<numeric_literal_t>(rhs_node).value == 1};
                        } else if (rhs_node.which() == variant::types::boolean_literal) {
                            rhs = boost::get<boolean_literal_t>(rhs_node);
                        } else {
                            error(result,
                                  "E005",
                                  "only boolean values can be used with logical operators");
                        }

                        if (result.is_failed())
                            break;

                        switch (op.op) {
                            case operator_t::op::equal:       return lhs == rhs;
                            case operator_t::op::not_equal:   return lhs == rhs;
                            case operator_t::op::logical_or:  return lhs || rhs;
                            case operator_t::op::logical_and: return lhs && rhs;
                            default:
                                error(result, "E008", fmt::format("operator {} is not supported", op.symbol));
                                break;
                        }
                        break;
                    }
                    default: {
                        error(result, "E008", "unknown binary operator group");
                        break;
                    }
                }
                break;
            }
            case ast_node_t::unary_op: {
                auto rhs_node = evaluate(result, node->rhs);
                auto op = boost::get<operator_t>(node->value);
                switch (op.group) {
                    case operator_t::arithmetic: {
                        numeric_literal_t rhs;
                        if (rhs_node.which() == variant::types::boolean_literal) {
                            rhs = numeric_literal_t {boost::get<boolean_literal_t>(rhs_node).value};
                        } else if (rhs_node.which() == variant::types::numeric_literal) {
                            rhs = boost::get<numeric_literal_t>(rhs_node);
                        } else {
                            error(result, "E005", "only integer values can be used with arithmetic operators");
                            break;
                        }
                        switch (op.op) {
                            case operator_t::op::negate: return -rhs;
                            case operator_t::op::invert: return ~rhs;
                            default:
                                error(result, "E008", fmt::format("operator {} is not supported", op.symbol));
                                break;
                        }
                        break;
                    }
                    case operator_t::logical: {
                        boolean_literal_t rhs;
                        if (rhs_node.which() == variant::types::numeric_literal) {
                            rhs = boolean_literal_t {boost::get<numeric_literal_t>(rhs_node).value == 1};
                        } else if (rhs_node.which() == variant::types::boolean_literal) {
                            rhs = boost::get<boolean_literal_t>(rhs_node);
                        } else {
                            error(result, "E005", "only boolean values can be used with logical operators");
                            break;
                        }
                        switch (op.op) {
                            case operator_t::op::logical_not: return !rhs;
                            default:
                                error(result, "E008", fmt::format("operator {} is not supported", op.symbol));
                                break;
                        }
                        break;
                    }
                    default:
                        error(result, "E008", "unknown unary operator group");
                        break;
                }
                break;
            }
        }

        return {};
    }

    core::symbol_table* evaluator::symbol_table() {
        return _symbol_table;
    }

    void evaluator::symbol_table(core::symbol_table* value) {
        _symbol_table = value;
    }

}