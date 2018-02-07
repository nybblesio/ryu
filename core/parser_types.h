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

#include <map>
#include <string>
#include <vector>
#include <ostream>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <boost/variant.hpp>
#include <boost/filesystem/path.hpp>

namespace ryu::core {

    class result;
    class environment;

    struct ast_node_t;

    typedef std::shared_ptr<ast_node_t> ast_node_shared_ptr;
    typedef std::vector<ast_node_shared_ptr> ast_node_list;
    typedef std::map<std::string, ast_node_shared_ptr> symbol_dict;

    struct command {
        enum types : uint8_t {
            quit = 1,
            help,
            clear,

            add_symbol,
            remove_symbol,
            show_symbol_table,

            peek,
            poke,
            assemble,
            evaluate,
            disassemble,
            dump_memory,
            search_memory,
            fill_memory,
            copy_memory,
            jump_to_address,
            go_to_address,
            register_editor,

            move_file,
            list_files,
            remove_file,
            make_directory,
            change_directory,
            print_working_directory,

            target,
            new_project,
            edit_project,
            load_project,
            save_project,
            close_project,
            clone_project,
            new_project_file,
            save_project_file,
            list_project_files,
            remove_project_file,

            new_environment,
            edit_environment,
            list_environments,
            remove_environment,
            switch_environment,

            list_machines,
            edit_machine,
            delete_machine,
            use_machine,
            memory_map,

            open_editor,
            source_editor,
            memory_editor,
            sprite_editor,
            tile_editor,
            background_editor,
            module_editor,
            sample_editor,

            read_binary_to_memory,
            write_memory_to_binary,
            read_text,
            write_text,
            goto_line,
            find_text
        };
    };
    struct variant {
        enum types {
            radix_numeric_literal = 0,
            numeric_literal,
            boolean_literal,
            identifier,
            string_literal,
            char_literal,
            operator_literal,
            command_literal,
            comment_literal,
            directive,
            label,
            dup_literal,

            // N.B. these must always be the last values
            variadic,
            any
        };

        static std::string to_string(types value) {
            switch (value) {
                case radix_numeric_literal: return "radix_numeric_literal";
                case numeric_literal:       return "numeric_literal";
                case boolean_literal:       return "boolean_literal";
                case identifier:            return "identifier";
                case string_literal:        return "string_literal";
                case char_literal:          return "char_literal";
                case operator_literal:      return "operator_literal";
                case command_literal:       return "command_literal";
                case comment_literal:       return "comment_literal";
                case directive:             return "directive";
                case label:                 return "label";
                case variadic:              return "variadic";
                case dup_literal:           return "dup_literal";
                case any:                   return "any";
            }
            return "unknown";
        }
    };

    struct command_parameter_spec_t {
        std::string name;
        variant::types type;
        bool required = true;
        bool evaluate = true;
    };

    enum command_size_flags : uint8_t {
        none =  0b00000000,
        byte =  0b00000001,
        word =  0b00000010,
        dword = 0b00000100,
        qword = 0b00001000
    };

    typedef uint8_t command_flags_t;

    struct command_spec_t {
        command::types type {};
        command_flags_t valid_sizes = command_size_flags::none;
        std::vector<command_parameter_spec_t> params {};
        std::string help {};
        std::string category = "system";
        uint32_t sequence {};
        std::string full_help;
        std::string command_name;
    };

    typedef std::vector<command_spec_t> command_spec_list;

    struct command_t {
        command_spec_t spec;
        std::string symbol;
        command_size_flags size = command_size_flags::dword;

        friend std::ostream& operator<<(
                std::ostream& stream,
                const command_t& command) {
            stream << command.symbol;
            switch (command.size) {
                case byte:
                    stream << ".b";
                    break;
                case word:
                    stream << ".w";
                    break;
                case dword:
                    stream << ".dw";
                    break;
                default:
                    break;
            }
            return stream;
        }

        static uint32_t size_to_byte_count(command_size_flags flags) {
            switch (flags) {
                default:    return 1;
                case word:  return 2;
                case dword: return 4;
                case qword: return 8;
            }
        }

        static command_size_flags token_to_size(const std::string& token) {
            if (token == "b")
                return command_size_flags::byte;
            else if (token == "w")
                return command_size_flags::word;
            else if (token == "dw") {
                return command_size_flags::dword;
            }
            return command_size_flags::none;
        }
    };

    struct directive_t {
        enum types {
            unknown,
            data,
            equate,
            origin,
            target,
            local,
            end_block,
            structure,
            align,
            if_block,
            elseif_block,
            else_block,
            loop,
            macro,
            include,
            binary
        };

        enum data_sizes {
            none,
            byte,
            word,
            dword,
            ascii
        };

        types type;
        data_sizes data_size;

        friend std::ostream& operator<<(
                std::ostream& stream,
                const directive_t& directive) {
            switch (directive.type) {
                case data: {
                    switch (directive.data_size) {
                        case directive_t::data_sizes::byte:
                            stream << ".byte";
                            break;
                        case directive_t::data_sizes::word:
                            stream << ".word";
                            break;
                        case directive_t::data_sizes::dword:
                            stream << ".dword";
                            break;
                        case directive_t::data_sizes::ascii:
                            stream << ".ascii";
                            break;
                        default:
                            break;
                    }
                    break;
                }
                case equate:
                    stream << ".equ";
                    break;
                case origin:
                    stream << ".org";
                    break;
                case target:
                    stream << ".target";
                    break;
                case local:
                    stream << ".local";
                    break;
                case end_block:
                    stream << ".end";
                    break;
                case structure:
                    stream << ".struct";
                    break;
                case align:
                    stream << ".align";
                    break;
                case if_block:
                    stream << ".if";
                    break;
                case elseif_block:
                    stream << ".elseif";
                    break;
                case else_block:
                    stream << ".else";
                    break;
                case loop:
                    stream << ".loop";
                    break;
                case macro:
                    stream << ".macro";
                    break;
                case include:
                    stream << ".include";
                    break;
                case binary:
                    stream << ".binary";
                    break;
                default:
                    stream << "***unknown***";
                    break;
            }
            return stream;
        }

        static directive_t::types type_for_directive(const std::string& name) {
            auto directive_type_it = _directive_to_type.find(name);
            if (directive_type_it != _directive_to_type.end())
                return directive_type_it->second;
            return directive_t::types::unknown;
        }

        static directive_t::data_sizes size_for_directive(const std::string& name) {
            if (name == "byte") {
                return directive_t::data_sizes::byte;
            } else if (name == "word") {
                return directive_t::data_sizes::word;
            } else if (name == "dword") {
                return directive_t::data_sizes::dword;
            } else if (name == "ascii") {
                return directive_t::data_sizes::ascii;
            }
            return directive_t::data_sizes::none;
        }

    private:
        static std::map<std::string, directive_t::types> _directive_to_type;
    };

    struct scanner_pos_t {
        uint32_t line;
        size_t index;
        uint32_t column;
    };

    using custom_parser_callable = std::function<ast_node_shared_ptr ()>;

    struct operator_t {
        enum op : uint16_t {
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
            logical_not,
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
            logical,
            conversion
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

        friend std::ostream& operator<<(std::ostream& stream, const operator_t& op) {
            stream << op.symbol;
            return stream;
        }

        uint16_t op;
        std::string symbol;
        uint8_t precedence = 0;
        uint8_t type = op_type::no_op;
        associativity_type associativity = associativity_type::none;
        op_group group = op_group::arithmetic;
        custom_parser_callable custom_parser;
    };

    struct radix_numeric_literal_t {
        uint8_t radix = 0;
        std::string value;

        enum conversion_result {
            success,
            overflow,
            underflow,
            inconvertible
        };

        conversion_result parse(uint32_t& out) const {
            const char* s = value.c_str();
            char* end;
            long l;
            errno = 0;
            l = strtol(s, &end, radix);
            if ((errno == ERANGE && l == LONG_MAX) || l > UINT_MAX) {
                return overflow;
            }
            if ((errno == ERANGE && l == LONG_MIN)) {
                return underflow;
            }
            if (*s == '\0' || *end != '\0') {
                return inconvertible;
            }
            out = static_cast<uint32_t>(l);
            return success;
        }

        friend std::ostream& operator<<(std::ostream& stream, const radix_numeric_literal_t& lit) {
            switch (lit.radix) {
                case 2:
                    stream << fmt::format("%{0}", lit.value);
                    break;
                case 8:
                    stream << fmt::format("@{0}", lit.value);
                    break;
                case 16:
                    stream << fmt::format("${0}", lit.value);
                    break;
                default:
                    stream << fmt::format("{0}", lit.value);
                    break;
            }
            return stream;
        }
    };

    struct comment_t {
        std::string value {};

        inline bool empty() const {
            return value.empty();
        }

        operator std::string() {
            return value;
        }

        operator std::string() const {
            return value;
        }

        friend std::ostream& operator<<(
                std::ostream& stream,
                const comment_t& comment) {
            stream << "* " << comment.value;
            return stream;
        }
    };

    struct identifier_t {
        std::string value {};

        inline bool empty() const {
            return value.empty();
        }

        operator std::string() {
            return value;
        }

        operator std::string() const {
            return value;
        }

        friend std::ostream& operator<<(
                std::ostream& stream,
                const identifier_t& identifier) {
            stream << identifier.value;
            return stream;
        }
    };

    struct label_t {
        std::string value {};

        inline bool empty() const {
            return value.empty();
        }

        operator std::string() {
            return value;
        }

        operator std::string() const {
            return value;
        }

        friend std::ostream& operator<<(
                std::ostream& stream,
                const label_t& label) {
            stream << label.value << ":";
            return stream;
        }
    };

    struct string_literal_t {
        std::string value {};

        inline bool empty() const {
            return value.empty();
        }

        operator std::string() {
            return value;
        }

        operator std::string() const {
            return value;
        }

        friend std::ostream& operator<<(
                std::ostream& stream,
                const string_literal_t& lit) {
            stream << "\"" << lit.value << "\"";
            return stream;
        }

        operator boost::filesystem::path() {
            return value;
        }

        operator boost::filesystem::path() const {
            return value;
        }
    };

    struct boolean_literal_t {
        bool value {};

        operator bool() {
            return value;
        }
        operator bool() const {
            return value;
        }
        operator uint32_t() {
            return value ? 1 : 0;
        }
        operator uint32_t() const {
            return value ? 1 : 0;
        }
        boolean_literal_t operator! () {
            return boolean_literal_t {!value};
        }
        boolean_literal_t operator== (const boolean_literal_t& other) {
            return boolean_literal_t {value == other.value};
        }
        boolean_literal_t operator!= (const boolean_literal_t& other) {
            return boolean_literal_t {value != other.value};
        }
        boolean_literal_t operator&& (const boolean_literal_t& other) {
            return boolean_literal_t {value && other.value};
        }
        boolean_literal_t operator|| (const boolean_literal_t& other) {
            return boolean_literal_t {value || other.value};
        }
        friend std::ostream& operator<<(std::ostream& stream, const boolean_literal_t& lit) {
            stream << std::boolalpha << lit.value;
            return stream;
        }
    };

    struct numeric_literal_t {
        uint32_t value {};

        operator uint32_t() {
            return value;
        }
        operator uint32_t() const {
            return value;
        }
        numeric_literal_t operator~ () {
            return numeric_literal_t {~value};
        }
        numeric_literal_t operator- () {
            return numeric_literal_t {
                static_cast<uint32_t>(-value)
            };
        }
        numeric_literal_t operator+ (const numeric_literal_t& other) {
            return numeric_literal_t {value + other.value};
        }
        numeric_literal_t operator- (const numeric_literal_t& other) {
            return numeric_literal_t {value - other.value};
        }
        numeric_literal_t operator* (const numeric_literal_t& other) {
            return numeric_literal_t {value * other.value};
        }
        numeric_literal_t operator/ (const numeric_literal_t& other) {
            return numeric_literal_t {value / other.value};
        }
        numeric_literal_t operator% (const numeric_literal_t& other) {
            return numeric_literal_t {value % other.value};
        }
        numeric_literal_t operator& (const numeric_literal_t& other) {
            return numeric_literal_t {value & other.value};
        }
        numeric_literal_t operator| (const numeric_literal_t& other) {
            return numeric_literal_t {value | other.value};
        }
        numeric_literal_t operator^ (const numeric_literal_t& other) {
            return numeric_literal_t {value ^ other.value};
        }
        boolean_literal_t operator< (const numeric_literal_t& other) {
            return boolean_literal_t {value < other.value};
        }
        boolean_literal_t operator<= (const numeric_literal_t& other) {
            return boolean_literal_t {value <= other.value};
        }
        boolean_literal_t operator== (const numeric_literal_t& other) {
            return boolean_literal_t {value == other.value};
        }
        boolean_literal_t operator!= (const numeric_literal_t& other) {
            return boolean_literal_t {value != other.value};
        }
        boolean_literal_t operator> (const numeric_literal_t& other) {
            return boolean_literal_t {value > other.value};
        }
        boolean_literal_t operator>= (const numeric_literal_t& other) {
            return boolean_literal_t {value >= other.value};
        }
        friend std::ostream& operator<<(
                std::ostream& stream,
                const numeric_literal_t& lit) {
            stream << lit.value;
            return stream;
        }
    };

    struct char_literal_t {
        unsigned char value {};

        operator unsigned char() {
            return value;
        }
        operator unsigned char() const {
            return value;
        }
        operator numeric_literal_t() {
            return numeric_literal_t {value};
        }
        operator numeric_literal_t() const {
            return numeric_literal_t {value};
        }
        boolean_literal_t operator== (const char_literal_t& other) {
            return boolean_literal_t {value == other.value};
        }
        boolean_literal_t operator!= (const char_literal_t& other) {
            return boolean_literal_t {value != other.value};
        }
        boolean_literal_t operator< (const char_literal_t& other) {
            return boolean_literal_t {value < other.value};
        }
        boolean_literal_t operator<= (const char_literal_t& other) {
            return boolean_literal_t {value <= other.value};
        }
        boolean_literal_t operator> (const char_literal_t& other) {
            return boolean_literal_t {value > other.value};
        }
        boolean_literal_t operator>= (const char_literal_t& other) {
            return boolean_literal_t {value >= other.value};
        }
        friend std::ostream& operator<<(
                std::ostream& stream,
                const char_literal_t& lit) {
            stream << "'" << lit.value << "'";
            return stream;
        }
    };

    struct dup_literal_t {
        uint32_t count;
        std::vector<numeric_literal_t> values;

        friend std::ostream& operator<<(
                std::ostream& stream,
                const dup_literal_t& lit) {
            return stream;
        }
    };

    typedef std::map<std::string, operator_t> operator_dict;

    typedef boost::variant<
        radix_numeric_literal_t,
        numeric_literal_t,
        boolean_literal_t,
        identifier_t,
        string_literal_t,
        char_literal_t,
        operator_t,
        command_t,
        comment_t,
        directive_t,
        label_t,
        dup_literal_t> variant_t;

    struct ast_node_t {
        enum tokens {
            program,
            command,
            comment,
            basic_block,
            statement,
            expression,
            binary_op,
            unary_op,
            identifier,
            label,
            string_literal,
            character_literal,
            number_literal,
            null_literal,
            boolean_literal,
            directive,
            parameter_list,
            branch,
            address,
            uninitialized_literal,
            location_counter_literal,
            redirection
        };

        void serialize(std::ostream& stream) {
            switch (token) {
                case redirection:
                    break;
                case branch:
                    lhs->serialize(stream);
                    if (rhs != nullptr)
                        rhs->serialize(stream);
                    break;
                case program:
                case basic_block:
                    for (const auto& child : children)
                        child->serialize(stream);
                    break;
                case parameter_list: {
                    auto child_count = children.size();
                    for (size_t i = 0; i < child_count; i++) {
                        const auto& child = children[i];
                        child->serialize(stream);
                        if (i < child_count - 1)
                            stream << ",";
                    }
                    break;
                }
                case expression:
                    stream << "(";
                    for (const auto& child : children)
                        child->serialize(stream);
                    stream << ")";
                    break;
                case binary_op:
                    lhs->serialize(stream);
                    stream << value;
                    rhs->serialize(stream);
                    break;
                case unary_op:
                    stream << value;
                    rhs->serialize(stream);
                    break;
                case statement:
                case directive: {
                    // XXX: custom macro serialization
                    if (lhs != nullptr)
                        lhs->serialize(stream);
                    stream << value << " ";
                    if (rhs != nullptr)
                        rhs->serialize(stream);
                    for (const auto& child : children)
                        child->serialize(stream);
                    break;
                }
                case label:
                case identifier:
                    stream << value;
                    break;
                case command:
                case address:
                case string_literal:
                case number_literal:
                case boolean_literal:
                case character_literal:
                    stream << value;
                    break;
                case comment:
                    stream << value;
                    break;
                case null_literal:
                    stream << "null";
                    break;
                case uninitialized_literal:
                    stream << "?";
                    break;
                case location_counter_literal:
                    stream << "$";
                    break;
            }
        }

        label_t label_type() const {
            return boost::get<label_t>(value);
        }

        command_t command_type() const {
            return boost::get<command_t>(value);
        }

        operator_t operator_type() const {
            return boost::get<operator_t>(value);
        }

        directive_t directive_type() const {
            return boost::get<directive_t>(value);
        }

        identifier_t identifier_type() const {
            return boost::get<identifier_t>(value);
        }

        char_literal_t char_literal_type() const {
            return boost::get<char_literal_t>(value);
        }

        string_literal_t string_literal_type() const {
            return boost::get<string_literal_t>(value);
        }

        boolean_literal_t boolean_literal_type() const {
            return boost::get<boolean_literal_t>(value);
        }

        numeric_literal_t numeric_literal_type() const {
            return boost::get<numeric_literal_t>(value);
        }

        radix_numeric_literal_t radix_numeric_literal_type() const {
            return boost::get<radix_numeric_literal_t>(value);
        }

        tokens token;
        variant_t value;
        ast_node_list children;
        ast_node_shared_ptr lhs = nullptr;
        ast_node_shared_ptr rhs = nullptr;
        ast_node_shared_ptr parent = nullptr;
        uint32_t line = 0;
        uint32_t column = 0;
    };

    typedef std::vector<core::variant_t> variant_list;
    typedef std::map<std::string, variant_list> command_parameter_dict;

    struct command_handler_context_t {
        core::result& result;
        core::command_t& command;
        core::command_parameter_dict& params;
        const core::ast_node_shared_ptr& root;

        variant_list get_variadic_parameters() const {
            auto it = params.find("...");
            if (it == params.end())
                return {};
            return it->second;
        }

        template <typename T>
        T get_parameter(const std::string& name) const {
            auto it = params.find(name);
            if (it == params.end())
                return {};
            return boost::get<T>(it->second.front());
        }
    };

    using command_handler_callable = std::function<bool (environment*, const command_handler_context_t&)>;

    typedef std::map<uint8_t, command_handler_callable> command_handler_dict;

    struct parser_input_t {
        static void split_to_lines(
                const std::string& source,
                std::vector<std::string>& source_lines) {
            source_lines.clear();
            std::stringstream stream;
            stream << source << "\n";
            std::string line;
            while (std::getline(stream, line)) {
                source_lines.push_back(line);
            }
        }

        parser_input_t() : source("") {
        }

        explicit parser_input_t(const std::string& source) {
            this->source = source;
            split_to_lines(this->source, this->source_lines);
        }

        inline bool empty() const {
            return source.empty();
        }

        inline size_t length() const {
            return source.length();
        }

        inline std::string& operator[](size_t index) {
            assert(index >= 0 && index < source_lines.size());
            return source_lines[index];
        }

        inline const std::string& operator[](size_t index) const {
            assert(index >= 0 && index < source_lines.size());
            return source_lines[index];
        }

        std::string source;
        std::vector<std::string> source_lines {};
    };
};