//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <iomanip>
#include <fmt/format.h>
#include <hardware/machine.h>
#include <hardware/registry.h>
#include <boost/filesystem.hpp>
#include <common/string_support.h>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include "command_factory.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "CannotResolve"

namespace ryu::ide {

    using namespace boost::spirit::qi;
    using namespace boost::fusion;

    // commands are not case sensitive
    //
    // !                                        - quit
    //
    // A                                        - assemble
    // A [addr]                                 - assemble
    //
    // ?                                        - expression evaluator
    // ? 'A'                                    - convert character to ASCII value
    // ? [number]                               - display various base values of number
    // ? [expression]                           - evaluate arithmetic expression
    //
    // E                                        - open the text editor
    // M [addr]                                 - open hex editor
    // D [addr]                                 - disassemble
    // H [addr]                                 - hex dump into console
    // S [addr] [bytes] [value]                 - search memory
    // F [addr] [bytes] [value]                 - fill memory
    // C [dst addr] [src addr] [bytes]          - copy memory
    //
    // J [addr]                                 - jump to address and begin execution
    // G [addr]                                 - go to address
    // R                                        - view/edit CPU registers
    //
    // CLEAR                                    - clear the console screen and move caret to home position
    // CD [path]                                - change directory
    // PWD                                      - print current directory
    // NEW [filename]                           - create a new project
    // LOAD [filename]                          - load an existing project from disk
    // SAVE                                     - save the open project to disk
    // CLONE [filename]                         - saves open project to a new file
    // RM [filename]                            - delete filename
    // DIR                                      - list contents of the current directory
    //
    // MACHINES                                 - list the machines available in the registry
    // MACHINE [name]                           - open editor for machine name or create a new machine
    // DELMACHINE [name]                        - remove the named machine from the registry
    // USEMACHINE [name]                        - set the machine as current for the project
    //
    // RB [filename] [start addr] [end addr]    - read binary into memory
    // WB [filename] [start addr] [end addr]    - write memory to binary file on disk
    //
    // RT [filename]                            - read text file into editor buffer
    // WT [filename]                            - write text in buffer to file
    // :  [number]                              - goto line number
    // /  [needle]                              - find needle in buffer
    //
    struct commands : symbols<char, uint8_t> {
        commands() {
            add
                    // assembler & memory commands
                    ("!",           command_factory::command::quit)
                    ("?",           command_factory::command::evaluate)
                    ("a",           command_factory::command::assemble)
                    ("e",           command_factory::command::text_editor)
                    ("m",           command_factory::command::memory_editor)
                    ("d",           command_factory::command::disassemble)
                    ("h",           command_factory::command::hex_dump)
                    ("s",           command_factory::command::search_memory)
                    ("f",           command_factory::command::fill_memory)
                    ("c",           command_factory::command::copy_memory)
                    ("j",           command_factory::command::jump_to_address)
                    ("g",           command_factory::command::go_to_address)
                    ("r",           command_factory::command::register_editor)
                    ("rb",          command_factory::command::read_binary_to_memory)
                    ("wb",          command_factory::command::write_memory_to_binary)

                    // filesystem commands
                    ("rm",          command_factory::command::remove_file)
                    ("dir",         command_factory::command::dir)
                    ("cd",          command_factory::command::change_directory)
                    ("pwd",         command_factory::command::print_working_directory)
                    ("clear",       command_factory::command::clear)

                    // project commands
                    ("new",         command_factory::command::new_project)
                    ("load",        command_factory::command::load_project)
                    ("save",        command_factory::command::save_project)
                    ("clone",       command_factory::command::clone_project)

                    // text editor commands
                    ("rt",          command_factory::command::read_text)
                    ("wt",          command_factory::command::write_text)
                    (":",           command_factory::command::goto_line)
                    ("/",           command_factory::command::find_text)

                    // machines
                    ("machines",    command_factory::command::machines_list)
                    ("machine",     command_factory::command::machine_editor)
                    ("delmachine",  command_factory::command::del_machine)
                    ("usemachine",  command_factory::command::use_machine)

                    // editor and tool commands
                    ("sounds",      command_factory::command::sounds)
                    ("tracker",     command_factory::command::tracker)
                    ("tiles",       command_factory::command::tile_editor)
                    ("sprites",     command_factory::command::sprite_editor)
                    ("backgrounds", command_factory::command::background_editor)
                    ;
            name("command-name");
        }
    };

    struct symbol_t {
        std::string value;
    };

    struct string_literal_t {
        std::string value;
    };

    typedef std::vector<boost::variant<uint32_t, symbol_t, string_literal_t, char>> command_parameters;
    struct command_t {
        command_factory::command::types type;
        command_parameters params;
    };

    struct error_reporter_t {
        template<typename, typename, typename, typename>
        struct result { typedef void type; };

        explicit error_reporter_t(std::string& error) : _error(error) {
        }

        template<typename Iter>
        void operator()(Iter first_iter, Iter last_iter, Iter error_iter, const info& what) const {
            std::string first(first_iter, error_iter);
            std::string last(error_iter, last_iter);

            auto first_pos = first.rfind('\n');
            auto last_pos = last.find('\n');
            auto error_line = ((first_pos == std::string::npos) ? first : std::string(first, first_pos + 1))
                              + std::string(last, 0, last_pos);
            auto error_pos = (error_iter - first_iter) + 1;
            if (first_pos != std::string::npos) {
                error_pos -= (first_pos + 1);
            }

            std::stringstream stream;
            stream << "Syntax error in " << what << std::endl
                   << error_line << std::endl
                   << std::setw(error_pos) << '^' << std::endl;
            const_cast<error_reporter_t*>(this)->_error = stream.str();
        }

    private:
        std::string& _error;
    };

    template <typename Iterator>
    struct command_grammar : grammar<Iterator, command_t(), locals<std::string>, ascii::space_type> {
        command_grammar() : command_grammar::base_type(start, "command"),
                            reporter(_error) {
            character %= lit("'") > char_ > lit("'");
            character.name("character");

            symbol %= lexeme[
                    (('_' | alpha) >> *(alnum))
            ];
            symbol.name("symbol");

            number %= lexeme[
                        (('$' | lit("0x")) >> hex_parser)
                    |   (('%' | lit("0b")) >> bin_parser)
                    |   (('@' | lit("0")) >> octal_parser)
                    |   (dec_parser)
            ];
            number.name("number");

            quoted_string %= lexeme['"' > +(char_ - '"') > '"'];
            quoted_string.name("quoted_string");

            start %= (eps > no_case[command_symbols]) >> *(symbol | number | character | quoted_string);
            start.name("command");

            boost::phoenix::function<error_reporter_t> f = reporter;
            on_error<fail>(start, f(_1, _2, _3, _4));
        }

        std::string what() const {
            return _error;
        }

    private:
        std::string _error;
        commands command_symbols;
        error_reporter_t reporter;
        rule<Iterator, char()> character;
        rule<Iterator, uint32_t()> number;
        rule<Iterator, symbol_t()> symbol;
        uint_parser<uint32_t, 2, 1, 32> bin_parser;
        uint_parser<uint32_t, 16, 1, 8> hex_parser;
        uint_parser<uint32_t, 10, 1, 10> dec_parser;
        uint_parser<uint32_t, 8, 1, 17> octal_parser;
        rule<Iterator, string_literal_t()> quoted_string;
        rule<Iterator, command_t(), locals<std::string>, ascii::space_type> start;
    };

    bool command_factory::execute(
            core::result& result,
            const std::string& line) {
        using namespace boost::filesystem;

        command_grammar<std::string::const_iterator> grammar;
        command_t command;

        if (!phrase_parse(line.begin(), line.end(), grammar, ascii::space, command)) {
            result.add_message("C999", grammar.what(), true);
            result.fail();
            return false;
        }

        switch (command.type) {
            case command::quit: {
                result.add_message("C001", "Goodbye!");
                break;
            }
            case command::clear: {
                result.add_message("C004", "Clear screen buffer");
                break;
            }
            case command::evaluate: {
                for (const auto& param : command.params) {
                    switch (param.which()) {
                        case 0:
                        case 3: {
                            std::string ascii;
                            uint32_t value = param.which() == 0 ?
                                             boost::get<uint32_t>(param) :
                                             static_cast<uint32_t>(boost::get<char>(param));
                            auto* p = (char*) (&value) + 3;
                            for (auto i = 0; i < 4; i++) {
                                char c = *p--;
                                if (c == 0)
                                    ascii += ".";
                                else
                                    ascii += c;
                            }
                            result.add_message(
                                    "C003",
                                    fmt::format("${0:08x} {0:>10} \"{1:>4}\" %{0:032b}", value, ascii));
                            break;
                        }
                        case 1: {
                            break;
                        }
                        case 2: {
                            string_literal_t lit = boost::get<string_literal_t>(param);
                            auto dump = hex_dump(
                                    static_cast<const void*>(lit.value.c_str()),
                                    lit.value.length());
                            result.add_message("C003", dump);
                            break;
                        }
                    }
                }
                break;
            }
            case command::change_directory: {
                if (command.params.empty()) {
                    result.add_message("C007", "path parameter is required", true);
                    return false;
                }
                string_literal_t lit = boost::get<string_literal_t>(command.params[0]);
                if (!is_directory(lit.value)) {
                    result.add_message("C007", fmt::format("invalid path: {}", lit.value), true);
                    return false;
                }
                current_path(lit.value);
                auto cwd = current_path();
                result.add_message("C007", fmt::format("working path is now: {}", cwd.string()));
                break;
            }
            case command::dir: {
                auto cwd = current_path();
                if (is_directory(cwd)) {
                    result.add_message("C005", fmt::format("{0}:", cwd.string()));
                    std::vector<directory_entry> entries;
                    copy(directory_iterator(cwd), directory_iterator(), back_inserter(entries));
                    for (auto& entry : entries) {
                        result.add_message("C005", fmt::format("  {0}", entry.path().filename().string()));
                    }
                }
                break;
            }
            case command::remove_file: {
                if (command.params.empty()) {
                    result.add_message("C008", "path parameter is required", true);
                    return false;
                }
                string_literal_t lit = boost::get<string_literal_t>(command.params[0]);
                if (!is_directory(lit.value) && !is_regular_file(lit.value)) {
                    result.add_message("C008", fmt::format("invalid path: {}", lit.value), true);
                    return false;
                }
                if (remove(lit.value)) {
                    result.add_message("C008", fmt::format("{} removed", lit.value));
                } else {
                    result.add_message("C008", fmt::format("remove of {} failed", lit.value), true);
                }
                break;
            }
            case command::print_working_directory: {
                auto cwd = current_path();
                if (is_directory(cwd)) {
                    result.add_message("C006", cwd.string());
                }
                break;
            }
            case command::read_text: {
                if (command.params.empty()) {
                    result.add_message("C021", "path parameter is required", true);
                    return false;
                }
                string_literal_t lit = boost::get<string_literal_t>(command.params[0]);
                if (!is_regular_file(lit.value)) {
                    result.add_message("C021", fmt::format("invalid path: {}", lit.value), true);
                    return false;
                }
                result.add_message("C021", lit.value);
                break;
            }
            case command::write_text: {
                string_literal_t lit {"(default)"};
                if (!command.params.empty()) {
                    lit = boost::get<string_literal_t>(command.params[0]);
                    if (!is_regular_file(lit.value)) {
                        result.add_message("C022", fmt::format("invalid path: {}", lit.value), true);
                        return false;
                    }
                }
                result.add_message("C022", lit.value);
                break;
            }
            case command::memory_editor: {
                result.add_data("C024", {});
                break;
            }
            case command::text_editor: {
                result.add_data("C002", {});
                break;
            }
            case command::machines_list: {
                auto machines = hardware::registry::instance()->machines();
                result.add_message("C028", "{rev}{bold} ID     Name                            ");
                for (auto machine : machines) {
                    result.add_message("C028", fmt::format("{:>6d} \"{:<32s}\"", machine->id(), machine->name()));
                }
                result.add_message("C028", fmt::format("{} registered machines", machines.size()));
                break;
            }
            case command::machine_editor: {
                if (command.params.empty()) {
                    result.add_message("C023", "machine name required", true);
                    return false;
                }
                const auto& param = command.params[0];
                if (param.which() == 2) {
                    string_literal_t lit = boost::get<string_literal_t>(param);
                    result.add_data("C023", {{"name", lit.value}});
                } else {
                    result.add_message("C023", "machine name must be a string literal", true);
                    return false;
                }
                break;
            }
            case command::use_machine: {
                if (command.params.empty()) {
                    result.add_message("C026", "machine name required", true);
                    return false;
                }
                const auto& param = command.params[0];
                if (param.which() == 2) {
                    string_literal_t lit = boost::get<string_literal_t>(param);
                    result.add_data("C026", {{"name", lit.value}});
                } else {
                    result.add_message("C026", "machine name must be a string literal", true);
                    return false;
                }
                break;
            }
            case command::del_machine: {
                if (command.params.empty()) {
                    result.add_message("C027", "machine name required", true);
                    return false;
                }
                const auto& param = command.params[0];
                if (param.which() == 2) {
                    string_literal_t lit = boost::get<string_literal_t>(param);
                    result.add_data("C027", {{"name", lit.value}});
                } else {
                    result.add_message("C027", "machine name must be a string literal", true);
                    return false;
                }
                break;
            }
            case command::goto_line: {
                if (command.params.empty()) {
                    result.add_message("C020", "line number is required", true);
                    return false;
                }
                const auto& param = command.params[0];
                if (param.which() == 0) {
                    uint32_t value = boost::get<uint32_t>(param);
                    result.add_message("C020", std::to_string(value));
                } else {
                    result.add_message("C020", "line number must be a number", true);
                    return false;
                }
                break;
            }
            default: {
                result.add_message("C400", "Command not implemented.", true);
                return false;
            }
        }

        return true;
    }

}

BOOST_FUSION_ADAPT_STRUCT(ryu::ide::symbol_t, value);

BOOST_FUSION_ADAPT_STRUCT(ryu::ide::string_literal_t, value);

BOOST_FUSION_ADAPT_STRUCT(ryu::ide::command_t, type, params);

#pragma clang diagnostic pop