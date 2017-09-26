//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <map>
#include <stack>
#include <list>
#include <common/SDL_FontCache.h>
#include <boost/variant.hpp>

namespace ryu::core {

    class engine;

    class timer;

    typedef std::vector<timer*> timer_list;
    typedef std::function<void (timer*)> timer_callable;

    class context;

    typedef std::map<int, context*> context_dict;

    class state;

    typedef std::map<std::string, std::string> parameter_dict;
    typedef std::function<bool (state*, const std::string&, const parameter_dict&)> state_transition_callable;

    struct state_t {
        state* instance;
    };

    typedef std::map<int, state_t> state_dict;

    typedef std::map<std::string, std::string> blackboard;

    struct command_t {
        enum types : uint8_t {
            quit = 1,
            clear,

            assemble,
            evaluate,
            disassemble,
            hex_dump,
            search_memory,
            fill_memory,
            copy_memory,
            jump_to_address,
            go_to_address,
            register_editor,

            dir,
            remove_file,
            change_directory,
            print_working_directory,

            new_project,
            load_project,
            save_project,
            clone_project,

            machine_editor,
            machines_list,
            del_machine,
            use_machine,

            open_editor,
            text_editor,
            memory_editor,
            sprite_editor,
            tile_editor,
            background_editor,
            tracker,
            sounds,

            read_binary_to_memory,
            write_memory_to_binary,
            read_text,
            write_text,
            goto_line,
            find_text
        };

        types type;
        std::string symbol;
    };

    struct scanner_pos_t {
        int line;
        int index;
        int column;
    };

    struct operator_t {
        enum op {
            invert,
            negate,
            pow,
            multiply,
            divide,
            modulo,
            add,
            subtract,
            binary_and,
            binary_or,
            greater_than,
            greater_than_equal,
            less_than,
            less_than_equal,
            equal,
            not_equal,
            logical_and,
            logical_or,
            left_parenthesis,
            right_parenthesis,
            left_bracket,
            right_bracket
        };

        enum op_type : uint8_t {
            no_op = 0b00000000,
            unary = 0b00000001,
            binary= 0b00000010
        };

        enum op_group : uint8_t {
            arithmetic,
            relational,
            grouping,
            logical
        };

        enum associativity_type {
            none = 1,
            right,
            left
        };

        bool not_right_associative() {
            return type != associativity_type::right;
        }

        int compare_precedence(const operator_t& other) {
            return precedence > other.precedence ? 1 : other.precedence == precedence ? 0 : -1;
        }

        op op;
        std::string symbol;
        uint8_t precedence = 0;
        uint8_t type = op_type::no_op;
        associativity_type associativity = associativity_type::none;
        op_group group = op_group::arithmetic;
    };

    struct radix_number_t {
        int radix = 0;
        std::string value;

        enum conversion_result {
            success,
            overflow,
            underflow,
            inconvertible
        };

        conversion_result parse(uint32_t& out) {
            const char* s = value.c_str();
            char* end;
            long l;
            errno = 0;
            l = strtol(s, &end, radix);
            if ((errno == ERANGE && l == LONG_MAX) || l > INT_MAX) {
                return overflow;
            }
            if ((errno == ERANGE && l == LONG_MIN) || l < INT_MIN) {
                return underflow;
            }
            if (*s == '\0' || *end != '\0') {
                return inconvertible;
            }
            out = static_cast<uint32_t>(l);
            return success;
        }
    };

    typedef std::map<std::string, operator_t> operator_dict;

    struct ast_node_t;

    typedef std::vector<ast_node_t*> ast_node_list;

    typedef boost::variant<radix_number_t, uint32_t, std::string, char, operator_t, command_t, bool> variant_t;

    typedef std::map<std::string, ast_node_t*> symbol_dict;

    struct ast_node_t {
        enum tokens {
            command,
            comment,
            basic_block,
            statement,
            expression,
            binary_op,
            unary_op,
            identifier,
            string_literal,
            character_literal,
            number_literal,
            null_literal,
            boolean_literal
        };

        tokens token;
        variant_t value;
        ast_node_list children;
        ast_node_t* lhs = nullptr;
        ast_node_t* rhs = nullptr;
    };

};