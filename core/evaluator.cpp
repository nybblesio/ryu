//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <fstream>
#include <iostream>
#include <hardware/machine.h>
#include "project.h"
#include "evaluator.h"
#include "assembler.h"
#include "project_file.h"
#include "symbol_table.h"

namespace ryu::core {

    evaluator::evaluator(core::assembler* assembler) : _assembler(assembler) {
    }

    void evaluator::error(
            core::result& result,
            const std::string& code,
            const std::string& message) {
        result.add_message(code, message, true);
        result.fail();
    }

    bool evaluator::pass1_transform_node(
            core::result& result,
            const core::ast_node_shared_ptr& node) {
        if (node == nullptr)
            return false;

        switch (node->token) {
            case ast_node_t::comment: {
                // XXX: remove from ast
                break;
            }
            case ast_node_t::basic_block: {
                for (auto const& block_child : node->children) {
                    pass1_transform_node(result, block_child);
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
                transform_identifier_node(result, node);
                break;
            }
            case ast_node_t::branch: {
                // N.B. currently a nop
                break;
            }
            case ast_node_t::directive: {
                transform_directive_node(result, node);
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
            const core::ast_node_shared_ptr& program_node) {
        if (_assembler == nullptr) {
            // XXX: add error about missing assembler
            return false;
        }

        if (program_node == nullptr) {
            // XXX: add error
            return false;
        }

        for (const auto& program_child : program_node->children) {
            if (!pass1_transform_node(result, program_child)) {
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
            case ast_node_t::redirection:
                break;
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
                auto identifier = node->identifier_type();
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
                        auto value = node->radix_numeric_literal_type();
                        if (value.parse(out) != radix_numeric_literal_t::conversion_result::success) {
                            error(result, "E004", "numeric conversion error");
                            break;
                        }
                        return numeric_literal_t{out};
                    }
                    case variant::types::char_literal: {
                        return numeric_literal_t {node->char_literal_type().value};
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
            case ast_node_t::uninitialized_literal: {
                break;
            }
            case ast_node_t::location_counter_literal: {
                if (_assembler != nullptr) {
                    return numeric_literal_t {_assembler->location_counter()};
                } else {
                    error(
                        result,
                        "E008",
                        "no valid assembler assigned to evaluator");
                }
                break;
            }
            case ast_node_t::expression: {
                return evaluate(result, node->children[0]);
            }
            case ast_node_t::binary_op: {
                auto op = node->operator_type();
                switch (op.group) {
                    case operator_t::op_group::conversion: {
                        switch (op.op) {
                            case assembler_parser::operators::dup: {
                                auto lhs_node = evaluate(result, node->lhs);
                                dup_literal_t dup_literal {boost::get<numeric_literal_t>(lhs_node).value};
                                ast_node_list& children = node->rhs->children;
                                for (size_t i = 0; i < children.size(); i++) {
                                    auto& child_node = children[i];
                                    auto variant = evaluate(result, child_node);
                                    if (variant.which() == variant::types::numeric_literal) {
                                        dup_literal
                                                .values
                                                .push_back(boost::get<numeric_literal_t>(variant));
                                    } else if (variant.which() == variant::types::char_literal) {
                                        dup_literal
                                                .values
                                                .push_back(numeric_literal_t{boost::get<char_literal_t>(variant).value});
                                    } else {
                                        error(
                                                result,
                                                "E001",
                                                "dup pattern values must be integer or character literals");
                                        break;
                                    }
                                }
                                return dup_literal;
                            }
                            case assembler_parser::operators::paste: {
                                std::stringstream stream;
                                node->lhs->serialize(stream);
                                node->rhs->serialize(stream);
                                return string_literal_t {stream.str()};
                            }
                            default: {
                                error(
                                    result,
                                    "E008",
                                    fmt::format("operator {} is not supported", op.symbol));
                                break;
                            }
                        }
                        break;
                    }
                    case operator_t::op_group::relational:
                    case operator_t::op_group::arithmetic: {
                        numeric_literal_t lhs;
                        numeric_literal_t rhs;

                        auto lhs_node = evaluate(result, node->lhs);
                        auto rhs_node = evaluate(result, node->rhs);

                        if (lhs_node.which() == variant::types::boolean_literal) {
                            lhs = numeric_literal_t {
                                    static_cast<uint32_t>(boost::get<boolean_literal_t>(lhs_node).value ? 1 : 0)
                            };
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
                            rhs = numeric_literal_t {
                                static_cast<uint32_t>(boost::get<boolean_literal_t>(rhs_node).value ? 1 : 0)
                            };
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
                            case operator_t::op::shift_left:            return lhs << rhs;
                            case operator_t::op::shift_right:           return lhs >> rhs;
                            default:
                                error(result, "E008", fmt::format("operator {} is not supported", op.symbol));
                                break;
                        }
                        break;
                    }
                    case operator_t::op_group::logical: {
                        boolean_literal_t lhs;
                        boolean_literal_t rhs;

                        auto lhs_node = evaluate(result, node->lhs);
                        auto rhs_node = evaluate(result, node->rhs);

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
                auto op = boost::get<operator_t>(node->value);
                switch (op.group) {
                    case operator_t::conversion: {
                        switch (op.op) {
                            case assembler_parser::operators::quote: {
                                std::stringstream stream;
                                stream << '"';
                                node->rhs->serialize(stream);
                                stream << '"';
                                return string_literal_t{stream.str()};
                            }
                            case assembler_parser::operators::offset: {
                                break;
                            }
                            default: {
                                error(result, "E008", fmt::format("operator {} is not supported", op.symbol));
                                break;
                            }
                        }
                        break;
                    }
                    case operator_t::arithmetic: {
                        auto rhs_node = evaluate(result, node->rhs);
                        numeric_literal_t rhs;
                        if (rhs_node.which() == variant::types::boolean_literal) {
                            rhs = numeric_literal_t {
                                static_cast<uint32_t>(boost::get<boolean_literal_t>(rhs_node).value ? 1 : 0)
                            };
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
                        auto rhs_node = evaluate(result, node->rhs);
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

    bool evaluator::transform_directive_node(
            core::result& result,
            const core::ast_node_shared_ptr& node) {
        if (node == nullptr)
            return false;

        auto& listing = _assembler->listing();

        auto directive = node->directive_type();
        switch (directive.type) {
            case directive_t::types::align: {
                auto variant = evaluate(result, node->rhs);
                if (variant.which() != variant::types::numeric_literal) {
                    listing.annotate_line_error(
                            node->line,
                            "align directive requires a numeric constant");
                    error(result, "E004", "align directive requires a numeric constant");
                    break;
                }
                auto size = boost::get<numeric_literal_t>(variant).value;
                if (size > 0 && (size & (size - 1)) == 0) {
                    _assembler->align(static_cast<uint8_t>(size));
                    listing.annotate_line(
                            node->line,
                            _assembler->location_counter(),
                            {},
                            assembly_listing::row_flags::address);
                } else {
                    listing.annotate_line_error(
                            node->line,
                            "align directive requires an integer power of 2 value");
                    error(result, "E004", "align directive requires an integer power of 2 value");
                }
                break;
            }
            case directive_t::types::origin: {
                auto variant = evaluate(result, node->rhs);
                if (variant.which() != variant::types::numeric_literal) {
                    listing.annotate_line_error(
                            node->line,
                            "origin directive requires a numeric constant");
                    error(result, "E004", "origin directive requires a numeric constant");
                    break;
                }
                if (!result.is_failed()) {
                    auto number = boost::get<numeric_literal_t>(variant).value;
                    _assembler->location_counter(static_cast<uint32_t>(number));
                    listing.annotate_line(
                            node->line,
                            _assembler->location_counter(),
                            {},
                            assembly_listing::row_flags::address);
                }
                break;
            }
            case directive_t::types::target: {
                auto variant = evaluate(result, node->rhs);
                if (variant.which() != variant::types::string_literal) {
                    listing.annotate_line_error(
                            node->line,
                            "target directive requires a string constant");
                    error(result, "E004", "target directive requires a string constant");
                    break;
                }
                if (!result.is_failed()) {
                    auto component_name = boost::get<string_literal_t>(variant).value;
                    if (_assembler->load_target(result, component_name)) {
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
                auto identifier_name = node->lhs->identifier_type().value;
                _assembler->symbol_table()->put(identifier_name, node->rhs);
                listing.annotate_line(
                        node->line,
                        0,
                        {},
                        assembly_listing::row_flags::none);
                break;
            }
            case directive_t::types::local: {
                listing.annotate_line(
                        node->line,
                        0,
                        {},
                        assembly_listing::row_flags::none);
                break;
            }
            case directive_t::types::macro:
            case directive_t::types::structure: {
                auto statement_node = node->rhs;
                auto parameter_list = statement_node->lhs;
                auto identifier_name = parameter_list->children[0]->identifier_type().value;
                _assembler
                        ->symbol_table()
                        ->put(identifier_name, statement_node);
                listing.annotate_line(
                        node->line,
                        0,
                        {},
                        assembly_listing::row_flags::none);
                break;
            }
            case directive_t::types::include: {
                auto variant = evaluate(result, node->rhs);
                if (variant.which() != variant::types::string_literal) {
                    listing.annotate_line_error(
                            node->line,
                            "include directive requires a string constant for the path");
                    error(result, "E004", "include directive requires a string constant for the path");
                    break;
                }

                auto path = boost::get<string_literal_t>(variant).value;
                auto project = core::project::instance();
                auto project_file = project->find_file(path);
                if (project_file == nullptr) {
                    listing.annotate_line_error(
                        node->line,
                        "include directive specifies invalid path");
                    error(result, "E004", "include directive specifies invalid path");
                    break;
                }

                std::stringstream source;
                if (!project_file->read_text(result, source)) {
                    listing.annotate_line_error(
                            node->line,
                            fmt::format("include failed for path: {}", project_file->full_path()));
                    error(
                        result,
                        "E004",
                        fmt::format("include failed for path: {}", project_file->full_path()));
                    break;
                }

                listing.annotate_line(
                        node->line,
                        0,
                        {},
                        assembly_listing::row_flags::none);

                auto source_text = source.str();
                _assembler->assemble_stream(result, parser_input_t{source_text});
                break;
            }
            case directive_t::types::binary: {
                auto variant = evaluate(result, node->rhs);
                if (variant.which() != variant::types::string_literal) {
                    listing.annotate_line_error(
                            node->line,
                            "binary directive requires a string constant for the path");
                    error(
                            result,
                            "E004",
                            "binary directive requires a string constant for the path");
                    break;
                }

                auto start_location_counter = _assembler->location_counter();
                auto project = core::project::instance();
                auto project_file = project->find_file(boost::get<string_literal_t>(variant).value);
                if (project_file == nullptr) {
                    listing.annotate_line_error(
                        node->line,
                        "binary directive specifies invalid path");
                    error(result, "E004", "binary directive specifies invalid path");
                    break;
                }

                byte_list data_bytes {};
                std::stringstream source;
                if (!project_file->read_binary(result, source)) {
                    listing.annotate_line_error(
                        node->line,
                        fmt::format("binary include failed for path: {}", project_file->full_path()));
                    error(
                        result,
                        "E004",
                        fmt::format("binary include failed for path: {}", project_file->full_path()));
                    break;
                }

                _assembler->load_binary_to_location_counter(
                    result,
                    data_bytes,
                    source,
                    0);

                if (!result.is_failed()) {
                    listing.annotate_line(
                            node->line,
                            start_location_counter,
                            data_bytes,
                              assembly_listing::row_flags::address
                            | assembly_listing::row_flags::binary);
                }

                break;
            }
            case directive_t::types::data: {
                byte_list data {};
                auto start_location_counter = _assembler->location_counter();

                if (node->lhs != nullptr) {
                    pass1_transform_node(result, node->lhs);
                }

                for (const auto& parameter_node : node->rhs->children) {
                    byte_list data_bytes {};
                    auto variant = evaluate(result, parameter_node);
                    if (result.is_failed()) {
                        auto messages = result.messages();
                        listing.annotate_line_error(
                                node->line,
                                messages[messages.size() - 1].message());
                    } else {
                        if (variant.which() == variant::types::string_literal) {
                            auto text = boost::get<string_literal_t>(variant).value;
                            data_bytes = _assembler->write_data(text);
                        } else if (variant.which() == variant::types::char_literal) {
                            auto char_byte = boost::get<char_literal_t>(variant).value;
                            data_bytes = _assembler->write_data(
                                    directive_t::data_sizes::byte,
                                    char_byte);
                        } else if (variant.which() == variant::types::dup_literal) {
                            auto pattern_index = 0;
                            auto dup_literal = boost::get<dup_literal_t>(variant);
                            for (size_t i = 0; i < dup_literal.count; i++) {
                                auto numeric_literal = dup_literal.values[pattern_index++];
                                if (pattern_index >= dup_literal.values.size())
                                    pattern_index = 0;
                                auto temp_bytes = _assembler->write_data(
                                        directive.data_size,
                                        static_cast<uint32_t>(numeric_literal.value));
                                for (auto b : temp_bytes)
                                    data_bytes.push_back(b);
                            }
                        } else if (parameter_node->token == ast_node_t::uninitialized_literal) {
                            _assembler->increment_location_counter(directive.data_size);
                        } else {
                            if (variant.which() != variant::types::numeric_literal) {
                                listing.annotate_line_error(
                                        node->line,
                                        "data directives require constant numeric values");
                                error(result, "E004", "data directives require constant numeric values");
                                break;
                            }
                            auto number = boost::get<numeric_literal_t>(variant).value;
                            data_bytes = _assembler->write_data(
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
                            start_location_counter,
                            data,
                            assembly_listing::row_flags::address);
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
                        pass1_transform_node(result, branch_node->lhs);
                    } else {
                        if (branch_node->rhs != nullptr)
                            pass1_transform_node(result, branch_node->rhs);
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
                pass1_transform_node(result, branch_node->lhs);
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

        return !result.is_failed();
    }

    bool evaluator::transform_identifier_node(
            core::result& result,
            const core::ast_node_shared_ptr& node) {
        if (node == nullptr)
            return false;

        auto& listing = _assembler->listing();

        std::string identifier_name;
        switch (node->token) {
            case ast_node_t::label:
                identifier_name = node->label_type().value;
                break;
            case ast_node_t::identifier:
                identifier_name = node->identifier_type().value;
                break;
            default:
                listing.annotate_line_error(
                        node->line,
                        "unknown assembler state; this should not happen");
                error(
                        result,
                        "E004",
                        "unknown assembler state; this should not happen");
                break;
        }

        auto address_node = std::make_shared<ast_node_t>();
        address_node->token = ast_node_t::tokens::address;
        address_node->value = numeric_literal_t {_assembler->location_counter()};
        address_node->lhs = node;
        _assembler
                ->symbol_table()
                ->put(identifier_name, address_node);

        return !result.is_failed();
    }

    core::symbol_table* evaluator::symbol_table() {
        return _symbol_table;
    }

    void evaluator::symbol_table(core::symbol_table* value) {
        _symbol_table = value;
    }

}