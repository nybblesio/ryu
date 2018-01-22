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
#include <fmt/format.h>
#include <boost/variant.hpp>
#include <common/SDL_FontCache.h>

namespace ryu::core {

    class result;

    class environment;

    class project_file;

    typedef std::vector<project_file> project_file_list;

    class project;

    typedef std::shared_ptr<core::project> project_shared_ptr;
    typedef std::map<std::string, std::string> project_props_dict;

    class engine;

    class timer;

    typedef std::vector<timer*> timer_list;
    typedef std::function<void (timer*)> timer_callable;

    class context;

    typedef std::map<int, context*> context_dict;

    class state;

    struct state_t {
        state* instance;
    };

    typedef std::map<int, state_t> state_dict;

    typedef std::map<std::string, std::string> blackboard;

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
                case variadic:              return "variadic";
                case any:                   return "any";
            }
            return "unknown";
        }
    };

    enum command_types : uint8_t {
        quit = 1,
        help,
        clear,

        add_symbol,
        remove_symbol,
        show_symbol_table,

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

        new_project,
        edit_project,
        load_project,
        save_project,
        close_project,
        clone_project,
        new_project_file,
        remove_project_file,
        list_project_files,

        list_machines,
        edit_machine,
        delete_machine,
        use_machine,

        open_editor,
        source_editor,
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
        command_types type;
        command_flags_t valid_sizes = command_size_flags::none;
        std::vector<command_parameter_spec_t> params;
        std::string help;
    };

    struct command_t {
        command_spec_t spec;
        std::string symbol;
        command_size_flags size = command_size_flags::dword;

        friend std::ostream& operator<<(
                std::ostream& stream,
                const command_t& command) {
            stream << command.symbol;
            return stream;
        }
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

        friend std::ostream& operator<<(std::ostream& stream, const operator_t& op) {
            stream << op.symbol;
            return stream;
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

        conversion_result parse(int32_t& out) const {
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
            out = static_cast<int32_t>(l);
            return success;
        }

        friend std::ostream& operator<<(std::ostream& stream, const radix_number_t& lit) {
            switch (lit.radix) {
                case 2:
                    stream << fmt::format("%{0}", lit.value);
                    break;
                case 8:
                    stream << fmt::format("@{0}", lit.value);
                    break;
                case 10:
                    stream << fmt::format("{0}", lit.value);
                    break;
                case 16:
                    stream << fmt::format("${0}", lit.value);
                    break;
            }
            return stream;
        }
    };

    struct comment_t {
        std::string value;

        friend std::ostream& operator<<(std::ostream& stream, const comment_t& comment) {
            stream << "; " << comment.value;
            return stream;
        }
    };

    struct identifier_t {
        std::string value;

        friend std::ostream& operator<<(std::ostream& stream, const identifier_t& identifier) {
            stream << identifier.value;
            return stream;
        }
    };

    struct string_literal_t {
        std::string value;

        friend std::ostream& operator<<(std::ostream& stream, const string_literal_t& lit) {
            stream << "\"" << lit.value << "\"";
            return stream;
        }
    };

    struct char_literal_t {
        char value;

        friend std::ostream& operator<<(std::ostream& stream, const char_literal_t& lit) {
            stream << "'" << lit.value << "'";
            return stream;
        }
    };

    struct boolean_literal_t {
        bool value;
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
        int32_t value;
        numeric_literal_t operator~ () {
            return numeric_literal_t {~value};
        }
        numeric_literal_t operator- () {
            return numeric_literal_t {-value};
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
        friend std::ostream& operator<<(std::ostream& stream, const numeric_literal_t& lit) {
            stream << lit.value;
            return stream;
        }
    };

    typedef std::map<std::string, operator_t> operator_dict;

    struct ast_node_t;
    typedef std::shared_ptr<ast_node_t> ast_node_shared_ptr;

    typedef std::vector<ast_node_shared_ptr> ast_node_list;

    typedef boost::variant<
            radix_number_t,
            numeric_literal_t,
            boolean_literal_t,
            identifier_t,
            string_literal_t,
            char_literal_t,
            operator_t,
            command_t,
            comment_t> variant_t;

    typedef std::map<std::string, ast_node_shared_ptr> symbol_dict;

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

        void serialize(std::ostream& stream) {
            switch (token) {
                case basic_block:
                case statement:
                    break;
                case expression:
                    stream << "(";
                    for (auto child : children)
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
                case command:
                case comment:
                case identifier:
                case string_literal:
                case number_literal:
                case boolean_literal:
                case character_literal:
                    stream << value;
                    break;
                case null_literal:
                    stream << "null";
                    break;
            }
        }

        tokens token;
        variant_t value;
        ast_node_list children;
        ast_node_shared_ptr lhs = nullptr;
        ast_node_shared_ptr rhs = nullptr;
    };

    struct alignment {
        struct horizontal {
            enum types {
                none,
                left,
                right,
                center
            };
        };

        struct vertical {
            enum types {
                none,
                top,
                middle,
                bottom
            };
        };

        static FC_AlignEnum to_font_align(horizontal::types value) {
            FC_AlignEnum align = FC_AlignEnum::FC_ALIGN_LEFT;
            switch (value) {
                case horizontal::none:
                case horizontal::left:
                    align = FC_AlignEnum::FC_ALIGN_LEFT;
                    break;
                case horizontal::right:
                    align = FC_AlignEnum::FC_ALIGN_RIGHT;
                    break;
                case horizontal::center:
                    align = FC_AlignEnum::FC_ALIGN_CENTER;
                    break;
            }
            return align;
        }
    };

    struct border {
        enum types {
            none,
            solid,
            dashed,
            rounded
        };
    };

    struct dock {
        enum styles : uint8_t {
            none,
            bottom,
            fill,
            left,
            right,
            top
        };
    };

    class view;

    typedef std::vector<view*> view_list;

    typedef std::map<std::string, std::vector<core::variant_t>> command_parameter_dict;

    struct command_handler_context_t {
        core::result& result;
        core::command_t& command;
        core::command_parameter_dict& params;
        const core::ast_node_shared_ptr& root;
    };

    using command_handler_callable = std::function<bool (
            environment*,
            const command_handler_context_t&)>;

    typedef std::map<uint8_t, command_handler_callable> command_handler_dict;

    struct data_table_column_t {
        enum format_options : uint8_t {
            none        = 0b00000000,
            style_codes = 0b00000001,
            word_wrap   = 0b00000010,
        };

        std::string text {};
        uint16_t min_width {};
        uint16_t max_width {};
        alignment::horizontal::types alignment = alignment::horizontal::types::left;
        uint8_t padding = 1;
        uint8_t options = format_options::none;
        uint16_t width {};
    };

    struct data_table_row_t {
        std::vector<std::string> columns {};
    };

    struct data_table_t {
        std::vector<data_table_column_t> headers {};
        std::vector<data_table_column_t> footers {};
        std::vector<data_table_row_t> rows {};
        uint8_t line_spacing = 0;
    };

    typedef boost::variant<
            data_table_t,
            std::string,
            uint32_t,
            bool> parameter_variant_t;

    enum parameter_dict_types {
        table = 0,
        string,
        integer32,
        boolean
    };

    typedef std::map<std::string, parameter_variant_t> parameter_dict;
    typedef std::function<bool (const std::string&, const parameter_dict&)> state_transition_callable;

};