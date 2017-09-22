//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#pragma once

#include <stack>
#include <vector>
#include <boost/variant.hpp>
#include <core/result.h>

namespace ryu::ide {

    struct scanner_pos_t {
        int line;
        int index;
        int column;
    };

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

    struct operator_t {
        enum op_type : uint8_t {
            no_op = 0b00000000,
            unary = 0b00000001,
            binary= 0b00000010
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

        std::string symbol;
        uint8_t precedence = 0;
        uint8_t type = op_type::no_op;
        associativity_type associativity = associativity_type::none;
    };

    struct radix_number_t {
        int radix = 0;
        std::string value;
    };

    typedef std::map<std::string, operator_t> operator_dict;

    struct ast_node_t;

    typedef std::vector<ast_node_t*> ast_node_list;

    typedef boost::variant<radix_number_t, std::string, char, operator_t, command_t> variant_t;

    typedef std::map<std::string, variant_t> symbol_table;

    typedef std::map<std::string, command_t::types> command_table;

    struct ast_node_t {
        enum tokens {
            command,
            comment,
            basic_block,
            expression,
            binary_op,
            unary_op,
            identifier,
            string_literal,
            character_literal,
            number_literal,
            boolean_literal
        };

        tokens token;
        variant_t value;
        ast_node_list children;
        ast_node_t* lhs = nullptr;
        ast_node_t* rhs = nullptr;
    };

    class command_parser {
    public:
        explicit command_parser(const std::string& input);

        ast_node_t* parse();

        const core::result& result() const;

        const variant_t* symbol(const std::string& name) const;

        void symbol(const std::string& name, const variant_t& value);

    protected:
        void pop_position();

        void push_position();

        void reset_position();

        void increment_line();

        char* current_token();

        ast_node_t* pop_operand();

        ast_node_t* parse_number();

        void forget_top_position();

        void consume_white_space();

        char* move_to_next_token();

        operator_t* pop_operator();

        operator_t* peek_operator();

        ast_node_t* parse_command();

        void clear_position_stack();

        operator_t* parse_operator();

        ast_node_t* parse_expression();

        ast_node_t* parse_identifier();

        void push_operator(operator_t* op);

        ast_node_t* parse_literal_string();

        void push_operand(ast_node_t* node);

        ast_node_t* parse_literal_character();

        void error(const std::string& code, const std::string& message);

    private:
        static command_table _commands;
        static operator_dict _operators;

        int _line;
        int _index;
        int _column;
        std::string _input;
        core::result _result;
        symbol_table _symbols;
        char* _token = nullptr;
        operator_t* _last_operator = nullptr;
        std::stack<ast_node_t*> _operand_stack;
        std::stack<operator_t*> _operator_stack;
        std::stack<scanner_pos_t> _position_stack;
    };

};

