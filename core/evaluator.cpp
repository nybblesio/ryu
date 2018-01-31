//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <iostream>
#include <hardware/machine.h>
#include "project.h"
#include "evaluator.h"
#include "assembler.h"
#include "symbol_table.h"

namespace ryu::core {

    void evaluator::error(
            core::result& result,
            const std::string& code,
            const std::string& message) {
        result.add_message(code, message, true);
        result.fail();
    }

    bool evaluator::pass1_transform_node(
            core::result& result,
            core::assembler* assembler,
            const core::ast_node_shared_ptr& node) {
        if (node == nullptr)
            return false;

        auto& listing = assembler->listing();

        switch (node->token) {
            case ast_node_t::comment: {
                // XXX: remove from ast
                break;
            }
            case ast_node_t::basic_block: {
                for (auto const& block_child : node->children) {
                    pass1_transform_node(result, assembler, block_child);
                }
                break;
            }
            case ast_node_t::unary_op:
            case ast_node_t::binary_op:
            case ast_node_t::statement:
            case ast_node_t::expression:
            case ast_node_t::null_literal:
            case ast_node_t::number_literal:
            case ast_node_t::string_literal:
            case ast_node_t::boolean_literal:
            case ast_node_t::character_literal:
                // call evaluate
                break;
            case ast_node_t::label:
            case ast_node_t::identifier: {
                std::string identifier_name;
                switch (node->token) {
                    case ast_node_t::label:
                        identifier_name = boost::get<label_t>(node->value).value;
                        break;
                    case ast_node_t::identifier:
                        identifier_name = boost::get<identifier_t>(node->value).value;
                        break;
                    default:
                        listing.annotate_line_error(
                                node->line,
                                "unknown assembler state; this should not happen");
                        error(result, "E004", "unknown assembler state; this should not happen");
                        break;
                }
                auto address_node = std::make_shared<ast_node_t>();
                address_node->token = ast_node_t::tokens::address;
                address_node->value = numeric_literal_t {assembler->location_counter()};
                address_node->lhs = node;
                assembler
                        ->symbol_table()
                        ->put(identifier_name, address_node);
                break;
            }
            case ast_node_t::branch: {
                // N.B. currently a nop
                break;
            }
            case ast_node_t::directive: {
                auto directive = boost::get<directive_t>(node->value);
                switch (directive.type) {
                    case directive_t::types::origin: {
                        auto value = evaluate(result, node->rhs);
                        if (value.which() != variant::types::numeric_literal) {
                            listing.annotate_line_error(
                                    node->line,
                                    "origin directive requires a numeric constant");
                            error(result, "E004", "origin directive requires a numeric constant");
                            break;
                        }
                        if (!result.is_failed()) {
                            auto number = boost::get<numeric_literal_t>(value).value;
                            assembler->location_counter(static_cast<uint32_t>(number));
                            listing.annotate_line(
                                    node->line,
                                    assembler->location_counter(),
                                    {},
                                    assembly_listing::row_flags::none);
                        }
                        break;
                    }
                    case directive_t::types::target: {
                        auto value = evaluate(result, node->rhs);
                        if (value.which() != variant::types::string_literal) {
                            listing.annotate_line_error(
                                    node->line,
                                    "target directive requires a string constant");
                            error(result, "E004", "target directive requires a string constant");
                            break;
                        }
                        if (!result.is_failed()) {
                            auto component_name = boost::get<string_literal_t>(value).value;
                            if (assembler->load_target(result, component_name)) {
                                listing.annotate_line(
                                        node->line,
                                        0,
                                        {},
                                        assembly_listing::row_flags::none);
                            } else {
                                auto messages = result.messages();
                                listing.annotate_line_error(
                                        node->line,
                                        messages[messages.size() - 1].message());
                            }
                        }
                        break;
                    }
                    case directive_t::types::equate: {
                        auto identifier_name = boost::get<identifier_t>(node->lhs->value).value;
                        assembler->symbol_table()->put(identifier_name, node->rhs);
                        listing.annotate_line(
                                node->line,
                                0,
                                {},
                                assembly_listing::row_flags::none);
                        break;
                    }
                    case directive_t::types::data: {
                        std::vector<uint8_t> data {};

                        if (node->lhs != nullptr) {
                            pass1_transform_node(result, assembler, node->lhs);
                        }

                        for (const auto& parameter_node : node->rhs->children) {
                            std::vector<uint8_t> data_bytes {};
                            auto value = evaluate(result, parameter_node);
                            if (result.is_failed()) {
                                auto messages = result.messages();
                                listing.annotate_line_error(
                                        node->line,
                                        messages[messages.size() - 1].message());
                            } else {
                                if (value.which() == variant::types::string_literal) {
                                    auto text = boost::get<string_literal_t>(value).value;
                                    data_bytes = assembler->write_data(text);
                                } else if (value.which() == variant::types::char_literal) {
                                    auto char_byte = boost::get<char_literal_t>(value).value;
                                    data_bytes = assembler->write_data(
                                            directive_t::data_sizes::byte,
                                            char_byte);
                                } else {
                                    if (value.which() != variant::types::numeric_literal) {
                                        listing.annotate_line_error(
                                                node->line,
                                                "data directives require constant numeric values");
                                        error(result, "E004", "data directives require constant numeric values");
                                        break;
                                    }
                                    auto number = boost::get<numeric_literal_t>(value).value;
                                    data_bytes = assembler->write_data(
                                            directive.data_size,
                                            static_cast<uint32_t>(number));
                                }

                                for (const auto d : data_bytes)
                                    data.push_back(d);
                            }
                        }

                        if (!result.is_failed()) {
                            listing.annotate_line(
                                    node->line,
                                    assembler->location_counter(),
                                    data,
                                    assembly_listing::row_flags::none);
                        }

                        break;
                    }
                    case directive_t::types::loop: {
                        listing.annotate_line(
                                node->line,
                                0,
                                {},
                                assembly_listing::row_flags::none);
                        break;
                    }
                    case directive_t::types::if_block:
                    case directive_t::types::elseif_block: {
                        auto branch_node = node->children[0];
                        auto variant = evaluate(result, node->rhs);
                        if (!result.is_failed()) {
                            auto is_true = boost::get<boolean_literal_t>(variant).value;
                            if (is_true) {
                                pass1_transform_node(result, assembler, branch_node->lhs);
                            } else {
                                if (branch_node->rhs != nullptr)
                                    pass1_transform_node(result, assembler, branch_node->rhs);
                            }
                            listing.annotate_line(
                                    node->line,
                                    0,
                                    {},
                                    assembly_listing::row_flags::none);
                         } else {
                            auto messages = result.messages();
                            listing.annotate_line_error(
                                    node->line,
                                    messages[messages.size() - 1].message());
                        }
                        break;
                    }
                    case directive_t::types::else_block: {
                        auto branch_node = node->children[0];
                        pass1_transform_node(result, assembler, branch_node->lhs);
                        listing.annotate_line(
                                node->line,
                                0,
                                {},
                                assembly_listing::row_flags::none);
                        break;
                    }
                    case directive_t::types::end_block: {
                        // N.B. currently a nop
                        break;
                    }
                    default: {
                        listing.annotate_line_error(
                                node->line,
                                "unknown assembler directive");
                        error(result, "E004", "unknown assembler directive");
                        break;
                    }
                }
                break;
            }
            default:
                error(result, "E001", "undefined assembler ast node");
                break;
        }

        return !result.is_failed();
    }

    bool evaluator::pass1_transform(
            core::result& result,
            core::assembler* assembler,
            const core::ast_node_shared_ptr& program_node) {
        if (program_node == nullptr || assembler == nullptr) {
            // XXX: add error
            return false;
        }

        // xxx: for debug
        program_node->serialize(std::cout);

        for (const auto& program_child : program_node->children) {
            if (!pass1_transform_node(result, assembler, program_child)) {
                break;
            }
        }

        return !result.is_failed();
    }

    variant_t evaluator::evaluate(
            core::result& result,
            const ast_node_shared_ptr& node) {
        if (node == nullptr)
            return {};

        switch (node->token) {
            case ast_node_t::comment:
            case ast_node_t::command:
            case ast_node_t::null_literal:
            case ast_node_t::string_literal:
            case ast_node_t::boolean_literal:
            case ast_node_t::character_literal:
                return node->value;
            case ast_node_t::label:
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
            case ast_node_t::address:
            case ast_node_t::number_literal: {
                switch (node->value.which()) {
                    case variant::types::radix_numeric_literal: {
                        uint32_t out = 0;
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
            case ast_node_t::branch:
            case ast_node_t::program:
            case ast_node_t::directive:
            case ast_node_t::statement:
            case ast_node_t::basic_block:
            case ast_node_t::parameter_list:
                break;
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
                            lhs = numeric_literal_t {boost::get<boolean_literal_t>(lhs_node).value != 0};
                        } else if (lhs_node.which() == variant::types::numeric_literal) {
                            lhs = boost::get<numeric_literal_t>(lhs_node);
                        } else if(lhs_node.which() == variant::types::char_literal) {
                            lhs = numeric_literal_t {boost::get<char_literal_t>(rhs_node).value};
                        } else {
                            error(result,
                                  "E005",
                                  "only integer values can be used with arithmetic/relational operators");
                        }
                        if (rhs_node.which() == variant::types::boolean_literal) {
                            rhs = numeric_literal_t {boost::get<boolean_literal_t>(rhs_node).value != 0};
                        } else if (rhs_node.which() == variant::types::numeric_literal) {
                            rhs = boost::get<numeric_literal_t>(rhs_node);
                        } else if(rhs_node.which() == variant::types::char_literal) {
                            rhs = numeric_literal_t {boost::get<char_literal_t>(rhs_node).value};
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
                            rhs = numeric_literal_t {boost::get<boolean_literal_t>(rhs_node).value != 0};
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