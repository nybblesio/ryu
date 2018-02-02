//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <fmt/format.h>
#include <core/evaluator.h>
#include <core/core_types.h>
#include <hardware/machine.h>
#include <hardware/registry.h>
#include <boost/filesystem.hpp>
#include <core/command_parser.h>
#include <common/string_support.h>
#include <boost/algorithm/string.hpp>
#include "assembler.h"
#include "environment.h"
#include "project_file.h"
#include "symbol_table.h"
#include "hex_formatter.h"
#include "text_formatter.h"
#include "project_file_type.h"

namespace ryu::core {

    command_handler_dict environment::_command_handlers = {
        {
            core::command::types::quit,
            std::bind(&environment::on_quit, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::help,
            std::bind(&environment::on_help, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::clear,
            std::bind(&environment::on_clear, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::add_symbol,
            std::bind(&environment::on_add_symbol, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::remove_symbol,
            std::bind(&environment::on_remove_symbol, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::show_symbol_table,
            std::bind(&environment::on_show_symbol_table, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::assemble,
            std::bind(&environment::on_assemble, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::evaluate,
            std::bind(&environment::on_evaluate, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::disassemble,
            std::bind(&environment::on_disassemble, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::dump_memory,
            std::bind(&environment::on_dump_memory, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::search_memory,
            std::bind(&environment::on_search_memory, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::fill_memory,
            std::bind(&environment::on_fill_memory, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::copy_memory,
            std::bind(&environment::on_copy_memory, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::jump_to_address,
            std::bind(&environment::on_jump_to_address, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::go_to_address,
            std::bind(&environment::on_go_to_address, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::register_editor,
            std::bind(&environment::on_register_editor, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::list_files,
            std::bind(&environment::on_list_files, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::remove_file,
            std::bind(&environment::on_remove_file, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::move_file,
            std::bind(&environment::on_move_file, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::make_directory,
            std::bind(&environment::on_make_directory, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::change_directory,
            std::bind(&environment::on_change_directory, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::print_working_directory,
            std::bind(&environment::on_print_working_directory, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::new_project,
            std::bind(&environment::on_new_project, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::edit_project,
            std::bind(&environment::on_edit_project, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::load_project,
            std::bind(&environment::on_load_project, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::save_project,
            std::bind(&environment::on_save_project, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::close_project,
            std::bind(&environment::on_close_project, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::clone_project,
            std::bind(&environment::on_clone_project, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::new_project_file,
            std::bind(&environment::on_new_project_file, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::save_project_file,
            std::bind(&environment::on_save_project_file, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::remove_project_file,
            std::bind(&environment::on_remove_project_file, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::list_project_files,
            std::bind(&environment::on_list_project_files, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::new_environment,
            std::bind(&environment::on_new_environment, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::remove_environment,
            std::bind(&environment::on_remove_environment, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::switch_environment,
            std::bind(&environment::on_switch_environment, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::list_environments,
            std::bind(&environment::on_list_environments, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::edit_machine,
            std::bind(&environment::on_edit_machine, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::list_machines,
            std::bind(&environment::on_list_machines, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::delete_machine,
            std::bind(&environment::on_delete_machine, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::use_machine,
            std::bind(&environment::on_use_machine, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::open_editor,
            std::bind(&environment::on_open_editor, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::source_editor,
            std::bind(&environment::on_source_editor, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::memory_editor,
            std::bind(&environment::on_memory_editor, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::sprite_editor,
            std::bind(&environment::on_sprite_editor, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::tile_editor,
            std::bind(&environment::on_tile_editor, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::background_editor,
            std::bind(&environment::on_background_editor, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::module_editor,
            std::bind(&environment::on_module_editor, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::sample_editor,
            std::bind(&environment::on_sample_editor, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::read_binary_to_memory,
            std::bind(&environment::on_read_binary_to_memory, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::write_memory_to_binary,
            std::bind(&environment::on_write_memory_to_binary, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::read_text,
            std::bind(&environment::on_read_text, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::write_text,
            std::bind(&environment::on_write_text, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::goto_line,
            std::bind(&environment::on_goto_line, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::find_text,
            std::bind(&environment::on_find_text, std::placeholders::_1, std::placeholders::_2)
        },
    };

    static void format_numeric_conversion(
            int32_t value,
            core::command_size_flags size,
            std::vector<std::string>& results) {

        std::string ascii;
        std::string signed_fmt_spec,
                decimal_fmt_spec,
                ascii_fmt_spec,
                binary_fmt_spec;

        uint32_t mask = 0;
        auto byte_count = 0;
        switch (size) {
            case core::command_size_flags::byte:
                byte_count = 1;
                mask = 0xff;
                signed_fmt_spec = "${0:02x}";
                decimal_fmt_spec = "{0:>3}";
                ascii_fmt_spec = "\"{0:>1}\"";
                binary_fmt_spec = "%{0:08b}";
                break;
            case core::command_size_flags::word:
                byte_count = 2;
                mask = 0xffff;
                signed_fmt_spec = "${0:04x}";
                decimal_fmt_spec = "{0:>5}";
                ascii_fmt_spec = "\"{0:>2}\"";
                binary_fmt_spec = "%{0:016b}";
                break;
            case core::command_size_flags::none:
            case core::command_size_flags::dword:
                byte_count = 4;
                mask = 0xffffffff;
                signed_fmt_spec = "${0:08x}";
                decimal_fmt_spec = "{0:>10}";
                ascii_fmt_spec = "\"{0:>4}\"";
                binary_fmt_spec = "%{0:032b}";
                break;
            default:
                // XXX: support for qword
                break;
        }

        auto* p = (char*) (&value) + (byte_count - 1);
        for (auto i = 0; i < byte_count; i++) {
            char c = *p--;
            ascii += isprint(c) ? c : '.';
        }

        auto signed_value = value & mask;

        results.push_back(fmt::format(signed_fmt_spec, signed_value));
        results.push_back(fmt::format(decimal_fmt_spec, value));
        results.push_back(fmt::format(ascii_fmt_spec, ascii));
        results.push_back(fmt::format(binary_fmt_spec, signed_value));
    }

    environment::environment() : _assembler(std::make_unique<core::assembler>()),
                                 _symbol_table(std::make_unique<core::symbol_table>()) {
    }

    environment::~environment() {
    }

    bool environment::load(
            core::result& result,
            std::iostream& stream) {
        return false;
    }

    bool environment::save(
            core::result& result,
            std::iostream& stream) {
        return false;
    }

    bool environment::execute(
            core::result& result,
            const std::string& line) {
        using namespace boost::filesystem;

        core::command_parser parser;
        parser.symbol_table(_symbol_table.get());

        auto root = parser.parse(line);
        if (root == nullptr || parser.result().is_failed()) {
            for (auto& msg : parser.result().messages())
                result.add_message(msg.code(), msg.message(), msg.is_error());
            result.fail();
            return false;
        }

        auto command = boost::get<core::command_t>(root->value);
        core::evaluator evaluator(_assembler.get());
        evaluator.symbol_table(_symbol_table.get());

        core::command_parameter_dict params;
        if (root->children.empty()) {
            for (const auto& param_spec : command.spec.params) {
                if (param_spec.required) {
                    result.add_message(
                            "C801",
                            fmt::format("the parameter <italic>'{}'<> is required.", param_spec.name),
                            true);
                    result.fail();
                }
            }
        } else {
            auto idx = 0;
            for (const auto& param_spec : command.spec.params) {
                std::vector<core::variant_t>* values = nullptr;

                if (param_spec.required) {
                    if (idx >= root->children.size()) {
                        result.add_message(
                                "C801",
                                fmt::format("the parameter <italic>'{}'<> is required.", param_spec.name),
                                true);
                        result.fail();
                        break;
                    }
                }

                if (idx >= root->children.size())
                    break;

                auto it = params.find(param_spec.name);
                if (it == params.end()) {
                    auto range = params.insert(std::make_pair(param_spec.name, std::vector<core::variant_t>{}));
                    it = range.first;
                }

                values = &it->second;

                auto value = param_spec.evaluate ?
                             evaluator.evaluate(result, root->children[idx]) :
                             root->children[idx]->value;

                if (result.is_failed())
                    break;

                if (param_spec.type != core::variant::types::variadic
                &&  param_spec.type != core::variant::types::any) {
                    if (value.which() != param_spec.type) {
                        result.add_message(
                                "C801",
                                fmt::format("the parameter <italic>'{}'<> must be of type <italic>'{}'<>.",
                                            param_spec.name,
                                            core::variant::to_string(param_spec.type)),
                                true);
                        result.fail();
                    }
                }

                if (param_spec.type == core::variant::types::variadic) {
                    values->push_back(value);

                    ++idx;
                    while (idx < root->children.size()) {
                        value = param_spec.evaluate ?
                                evaluator.evaluate(result, root->children[idx++]) :
                                root->children[idx]->value;
                        if (result.is_failed())
                            break;
                        values->push_back(value);
                    }
                } else {
                    values->push_back(value);
                    ++idx;
                }
            }
        }

        if (result.is_failed())
            return false;

        auto handler_it = _command_handlers.find(command.spec.type);
        if (handler_it == _command_handlers.end()) {
            result.add_message("C400", "Command not implemented.", true);
            return false;
        }

        return handler_it->second(this, {result, command, params, root});
    }

    std::string environment::name() const {
        if (_name.empty())
            return "(none)";
        return _name;
    }

    core::symbol_table* environment::symbol_table() {
        return _symbol_table.get();
    }

    void environment::name(const std::string& value) {
        _name = value;
    }

    data_table_t environment::create_symbol_table() {
        data_table_t table {};

        table.headers.push_back({"Identifier",   40, 40});
        table.headers.push_back({"Expression",   40, 75});
        table.footers.push_back({"Symbol Count", 49, 64});

        auto identifiers = _symbol_table->identifiers();
        for (const auto& symbol : identifiers) {
            std::stringstream stream;
            _symbol_table->get(symbol)->serialize(stream);
            table.rows.push_back({{symbol, stream.str()}});
        }

        table.rows.push_back({
            {fmt::format("{} symbols defined", identifiers.size())}
        });

        return table;
    }

    bool environment::assemble(core::result& result) {
        if (core::project::instance() == nullptr) {
            result.add_message(
                    "A001",
                    "no project is loaded; assemble failed",
                    true);
            return false;
        }

        _assembler->symbol_table(_symbol_table.get());

        auto files = core::project::instance()->files();
        for (auto& file : files) {
            if (!file.should_assemble())
                continue;

            std::stringstream source;
            if (!file.read(result, source))
                break;

            auto source_text = source.str();
            if (!_assembler->assemble(result, source_text))
                break;
        }

        result.add_data(
            "command_result",
            {
                    {"listing", _assembler->listing().table()},
                    {"symbols", create_symbol_table()}
            });

        return !result.is_failed();
    }

    // ----------------------------------------------------------
    // system commands
    // ----------------------------------------------------------
    bool environment::on_quit(
            const command_handler_context_t& context) {
        context.result.add_data(
                "command_action",
                {{"action", std::string("quit")}});
        return true;
    }

    bool environment::on_help(
            const command_handler_context_t& context) {
        using format_options = core::data_table_column_t::format_options;

        const auto commands = core::command_parser::command_catalog();

        data_table_t table{};
        table.line_spacing = 1;
        table.headers.push_back({
                "Command",
                20,
                40,
                alignment::horizontal::left,
                1,
                format_options::style_codes
        });
        table.headers.push_back({
                "Help",
                35,
                75,
                alignment::horizontal::left,
                1,
                format_options::style_codes | format_options::word_wrap
        });
        table.footers.push_back({"Command Count", 15, 20});

        for (const auto& c : commands) {
            data_table_row_t row{};

            std::stringstream stream;
            stream << c.first;

            if (c.second.valid_sizes != core::command_size_flags::none) {
                stream << "<italic>[.";

                if ((c.second.valid_sizes & core::command_size_flags::byte) != 0) {
                    stream << "b";
                }

                if ((c.second.valid_sizes & core::command_size_flags::word) != 0) {
                    stream << "|w";
                }

                if ((c.second.valid_sizes & core::command_size_flags::dword) != 0) {
                    stream << "|dw";
                }

                if ((c.second.valid_sizes & core::command_size_flags::qword) != 0) {
                    stream << "|qw";
                }

                stream << "]<>";
            }

            if (!c.second.params.empty()) {
                stream << " ";
                for (size_t i = 0; i < c.second.params.size(); i++) {
                    auto param_spec = c.second.params[i];
                    if (param_spec.required) {
                        stream << "<bold>" << param_spec.name << "<>";
                    } else {
                        stream << "<italic>[" << param_spec.name << "]<>";
                    }
                    if (i < c.second.params.size() - 1)
                        stream << " ";
                }
            }

            row.columns.push_back(stream.str());
            row.columns.push_back(c.second.help);

            table.rows.push_back(row);
        }

        table.rows.push_back({
                {fmt::format("{} available commands", commands.size())}
        });

        context.result.add_data(
                "command_result",
                {{"data", table}});

        return true;
    }

    bool environment::on_clear(
            const command_handler_context_t& context) {
        context.result.add_data(
                "command_action",
                {{"action", std::string("clear")}});
        return true;
    }

    bool environment::on_open_editor(
            const command_handler_context_t& context) {
        core::parameter_dict dict;

        auto name = boost::get<core::string_literal_t>(context.params["name"].front()).value;
        auto type = boost::get<core::identifier_t>(context.params["type"].front()).value;

        context.result.add_data("command_action", {
                {"action", project_file_type::code_to_action(type)},
                {"name", name}
        });

        return true;
    }

    // ----------------------------------------------------------
    // environment commands
    // ----------------------------------------------------------
    bool environment::on_assemble(
            const command_handler_context_t& context) {
        return assemble(context.result);
    }

    bool environment::on_add_symbol(
            const command_handler_context_t& context) {
        auto identifier = boost::get<core::identifier_t>(context.params["name"].front()).value;
        _symbol_table->put(identifier, context.root->children[1]);
        return true;
    }

    bool environment::on_disassemble(
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_remove_symbol(
            const command_handler_context_t& context) {
        auto identifier = boost::get<core::identifier_t>(context.params["name"].front()).value;
        _symbol_table->remove(identifier);
        return true;
    }

    bool environment::on_show_symbol_table(
            const command_handler_context_t& context) {
        context.result.add_data(
                "command_result",
                {{"data", create_symbol_table()}});

        return true;
    }

    // XXX: support registers as valid identifiers
    bool environment::on_evaluate(
            const command_handler_context_t& context) {
        data_table_t conversion_table {};
        conversion_table.headers.push_back({"Hex",          6, 12});
        conversion_table.headers.push_back({"Dec",          6, 10});
        conversion_table.headers.push_back({"ASCII",        6,  6});
        conversion_table.headers.push_back({"Bin",          9, 33});
        conversion_table.footers.push_back({"Expressions", 17, 71});

        auto dump_count = 0;
        parameter_dict params;

        const auto& values = context.params["..."];
        for (const auto& param : values) {
            switch (param.which()) {
                case core::variant::types::char_literal: {
                    auto value = boost::get<core::char_literal_t>(param).value;
                    std::vector<std::string> results;
                    format_numeric_conversion(
                            value,
                            context.command.size,
                            results);
                    conversion_table.rows.push_back({results});
                    break;
                }
                case core::variant::types::numeric_literal: {
                    auto value = boost::get<core::numeric_literal_t>(param).value;
                    std::vector<std::string> results;
                    format_numeric_conversion(
                            value,
                            context.command.size,
                            results);
                    conversion_table.rows.push_back({results});
                    break;
                }
                case core::variant::types::string_literal: {
                    auto value = boost::get<core::string_literal_t>(param).value;
                    std::vector<std::vector<std::string>> lines;
                    auto table = core::hex_formatter::dump_to_table(
                            static_cast<const void*>(value.c_str()),
                            value.length());
                    params.insert(std::make_pair(
                            fmt::format("dump{}", ++dump_count),
                            table));
                    break;
                }
                case core::variant::types::boolean_literal: {
                    auto value = boost::get<core::boolean_literal_t>(param).value;
                    std::vector<std::string> results;
                    format_numeric_conversion(
                            value,
                            core::command_size_flags::byte,
                            results);
                    conversion_table.rows.push_back({results});
                    break;
                }
                default: {
                    break;
                }
            }
        }

        conversion_table.rows.push_back({{fmt::format("{} expression evaluated", values.size())}});
        params["data"] = conversion_table;

        context.result.add_data(
                "command_result",
                params);

        return true;
    }

    // ----------------------------------------------------------
    // memory commands
    // ----------------------------------------------------------
    bool environment::on_copy_memory(
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_fill_memory(
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_dump_memory(
            const command_handler_context_t& context) {
        if (core::project::instance() == nullptr) {
            context.result.add_message(
                    "C033",
                    "no project is loaded; dump memory failed",
                    true);
            return false;
        }

        auto buffer = new uint8_t[128];
        auto addr = boost::get<core::numeric_literal_t>(context.params["addr"].front()).value;

        auto machine = core::project::instance()->machine();
        if (machine == nullptr) {
            context.result.add_message(
                    "C033",
                    "no machine active on loaded project; dump memory failed",
                    true);
            return false;
        }

        for (size_t i = 0; i < 128; i++) {
            buffer[i] = machine
                    ->mapper()
                    ->read_byte(static_cast<uint32_t>(addr + i));
        }

        context.result.add_data(
            "command_result",
            {
                {
                    "data",
                    core::hex_formatter::dump_to_table(buffer, 128, 16)
                }
            });

        delete[] buffer;

        return true;
    }

    bool environment::on_search_memory(
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_memory_editor(
            const command_handler_context_t& context) {
        if (core::project::instance() == nullptr) {
            context.result.add_message(
                    "C033",
                    "no project is loaded; unable to open memory editor",
                    true);
            return false;
        }

        context.result.add_data("command_action", {{"action", std::string("edit_memory")}});
        return true;
    }

    // this command jumps to the specified address like
    // a subroutine jump (bsr/jsr).  If no address is specified, a
    // jump will be made to the first address in your program.
    //
    // Example: To jump to the label in your program named "start", type:
    //
    // j start
    bool environment::on_jump_to_address(
            const command_handler_context_t& context) {
        return true;
    }

    // this command is similar to jump, except only a breakpoint or
    // an illegal command can stop this.
    bool environment::on_go_to_address(
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_read_binary_to_memory(
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_write_memory_to_binary(
            const command_handler_context_t& context) {
        return true;
    }

    // ----------------------------------------------------------
    // file system commands
    // ----------------------------------------------------------
    bool environment::on_move_file(
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        auto src = boost::get<core::string_literal_t>(context.params["src"].front()).value;
        auto dest = boost::get<core::string_literal_t>(context.params["dest"].front()).value;

        boost::system::error_code ec;
        boost::filesystem::rename(src, dest, ec);
        if (ec) {
            context.result.add_message(
                    "C007",
                    fmt::format("move/rename failed: {}", ec.message()),
                    true);
            return false;
        }

        return true;
    }

    bool environment::on_list_files(
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        data_table_t table {};
        table.headers.push_back({"Filename",   11, 41});
        table.headers.push_back({"Size",       10, 16, alignment::horizontal::right});
        table.footers.push_back({"File Count", 10, 20});

        std::vector<directory_entry> entries;
        std::vector<directory_entry*> dir_entries;
        std::vector<directory_entry*> file_entries;

        auto cwd = current_path();
        if (is_directory(cwd)) {
            auto format_entries = [&](const std::vector<directory_entry*>& list) {
                for (auto entry : list) {
                    std::pair<std::string, std::string> size;
                    if (boost::filesystem::is_regular(entry->path()))
                        size = ryu::size_to_units(boost::filesystem::file_size(entry->path()));
                    data_table_row_t row {};
                    row.columns.push_back(fmt::format("\"{}\"", entry->path().filename().string()));
                    row.columns.push_back(fmt::format("{:>10} {:<5}", size.first, size.second));
                    table.rows.push_back(row);
                }
            };
            copy(directory_iterator(cwd), directory_iterator(), back_inserter(entries));
            for (auto& entry : entries) {
                if (entry.status().type() == file_type::directory_file) {
                    dir_entries.push_back(&entry);
                } else {
                    file_entries.push_back(&entry);
                }
            }
            std::sort(dir_entries.begin(), dir_entries.end());
            std::sort(file_entries.begin(), file_entries.end());
            format_entries(dir_entries);
            format_entries(file_entries);
        }

        table.rows.push_back({
            {fmt::format("{} files", dir_entries.size() + file_entries.size())}
        });

        context.result.add_data("command_result", {{"data", table}});

        return true;
    }

    bool environment::on_remove_file(
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        std::string message;
        auto value = boost::get<core::string_literal_t>(context.params["path"].front()).value;
        if (!is_directory(value) && !is_regular_file(value)) {
            context.result.add_message(
                    "C008",
                    fmt::format("invalid path: {}", value), true);
        } else {
            boost::system::error_code ec;

            if (remove(value, ec)) {
                context.result.add_message(
                        "C008",
                        fmt::format("removal success: {}", value));
            } else {
                context.result.add_message(
                        "C008",
                        fmt::format("removal failed: {}",
                                    ec.message()),
                        true);
            }
        }

        return !context.result.is_failed();
    }

    bool environment::on_make_directory(
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        auto value = boost::get<core::string_literal_t>(context.params["path"].front()).value;
        boost::system::error_code ec;

        if (!create_directory(value, ec)) {
            context.result.add_message(
                    "C007",
                    fmt::format("create directory failed: {}", ec.message()),
                    true);
            return false;
        }

        return true;
    }

    bool environment::on_change_directory(
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        auto value = boost::get<core::string_literal_t>(context.params["path"].front()).value;
        if (!is_directory(value)) {
            context.result.add_message(
                    "C007",
                    fmt::format("invalid path: {}", value),
                    true);
            return false;
        }

        current_path(value);

        context.result.add_data(
                "command_action",
                {
                    {"action", std::string("update_working_directory")},
                });

        return true;
    }

    bool environment::on_print_working_directory(
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        auto cwd = current_path();
        if (is_directory(cwd)) {
            context.result.add_message("C006", cwd.string());
        }
        return true;
    }

    // ----------------------------------------------------------
    // project & project_file commands
    // ----------------------------------------------------------
    bool environment::on_new_project(
            const command_handler_context_t& context) {
        return core::project::create(
                context.result,
                boost::get<core::string_literal_t>(context.params["path"].front()).value);
    }

    bool environment::on_edit_project(
            const command_handler_context_t& context) {
        context.result.add_data(
                "command_action",
                {{"action", std::string("edit_project")}});
        return true;
    }

    bool environment::on_load_project(
            const command_handler_context_t& context) {
        fs::path path;
        auto it = context.params.find("path");
        if (it != context.params.end())
            path = boost::get<core::string_literal_t>(it->second.front()).value;
        return core::project::load(context.result, path);
    }

    bool environment::on_save_project(
            const command_handler_context_t& context) {
        if (core::project::instance() == nullptr) {
            context.result.add_message(
                    "C033",
                    "no project is loaded; save failed",
                    true);
            return false;
        }
        return core::project::instance()->save(context.result);
    }

    bool environment::on_close_project(
            const command_handler_context_t& context) {
        return core::project::close(context.result);
    }

    bool environment::on_clone_project(
            const command_handler_context_t& context) {
        return core::project::clone(
                context.result,
                core::project::instance()->path(),
                boost::get<core::string_literal_t>(context.params["path"].front()).value);
    }

    bool environment::on_save_project_file(
            const command_handler_context_t& context) {
        if (core::project::instance() == nullptr) {
            context.result.add_message(
                    "C032",
                    "no project is loaded; unable to save project file",
                    true);
            return false;
        }

        context.result.add_data(
                "command_action",
                {{"action", std::string("save_project_file")}});

        return true;
    }

    bool environment::on_new_project_file(
            const command_handler_context_t& context) {
        if (core::project::instance() == nullptr) {
            context.result.add_message(
                "C032",
                "no project is loaded; unable to create project file",
                true);
            return false;
        }

        std::string type("TEXT");
        auto path = boost::get<core::string_literal_t>(context.params["path"].front()).value;
        auto type_it = context.params.find("type");
        if (type_it != context.params.end()) {
            type = boost::get<core::identifier_t>(type_it->second.front()).value;
        }

        core::project_file file(
                core::id_pool::instance()->allocate(),
                path,
                core::project_file_type::code_to_type(type));
        core::project::instance()->add_file(file);
        file.create_stub_file(context.result, path);

        return !context.result.is_failed();
    }

    bool environment::on_remove_project_file(
            const command_handler_context_t& context) {
        if (core::project::instance() == nullptr) {
            context.result.add_message(
                    "C032",
                    "no project is loaded; unable to remove project file",
                    true);
            return false;
        }

        auto path = boost::get<core::string_literal_t>(context.params["path"].front()).value;
        auto file = core::project::instance()->find_file(path);
        if (file == nullptr) {
            context.result.add_message(
                    "C032",
                    fmt::format("no project file exists: {}", path),
                    true);
            return false;
        }

        core::project::instance()->remove_file(file->id());

        return true;
    }

    bool environment::on_list_project_files(
            const command_handler_context_t& context) {
        if (core::project::instance() == nullptr) {
            context.result.add_message(
                "C032",
                "no project is loaded; unable to list files",
                true);
            return false;
        }

        data_table_t table {};
        table.headers.push_back({"ID",           5,  5});
        table.headers.push_back({"Path",        20, 50});
        table.headers.push_back({"Type",         8,  8});
        table.footers.push_back({"File Count",  15,  20});

        auto project_files = core::project::instance()->files();
        for (const auto& file : project_files) {
            if (file.type() == project_file_type::environment)
                continue;
            data_table_row_t row {};
            row.columns.push_back(std::to_string(file.id()));
            row.columns.push_back(fmt::format("\"{}\"", file.path().string()));
            row.columns.push_back(boost::to_upper_copy<std::string>(core::project_file_type::type_to_code(file.type())));
            table.rows.push_back(row);
        }

        table.rows.push_back({{fmt::format("{} project files", table.rows.size())}});

        context.result.add_data(
                "command_result",
                {{"data", table}});

        return true;
    }

    // ----------------------------------------------------------
    // machine commands
    // ----------------------------------------------------------
    bool environment::on_use_machine(
            const command_handler_context_t& context) {
        if (core::project::instance() == nullptr) {
            context.result.add_message(
                    "C034",
                    "no project is loaded; use machine failed",
                    true);
            return false;
        }
        auto machine_name = boost::get<core::string_literal_t>(context.params["name"].front()).value;
        auto machine = hardware::registry::instance()->find_machine(machine_name);
        if (machine == nullptr) {
            context.result.add_message(
                    "C034",
                    "no machine exists with that name",
                    true);
            return false;
        }
        core::project::instance()->machine(machine);
        return !context.result.is_failed();
    }

    bool environment::on_edit_machine(
            const command_handler_context_t& context) {
        context.result.add_data(
                "command_action",
                {
                    {"action", std::string("edit_machine")},
                    {"name", boost::get<core::string_literal_t>(context.params["name"].front()).value}
                });
        return true;
    }

    bool environment::on_list_machines(
            const command_handler_context_t& context) {
        auto machines = hardware::registry::instance()->machines();

        data_table_t table {};
        table.headers.push_back({"ID",              5,   5});
        table.headers.push_back({"Name",           12,  32});
        table.headers.push_back({"Type",            4,   4});
        table.footers.push_back({"Machine Count",  15,  20});

        for (auto machine : machines) {
            data_table_row_t row {};
            row.columns.push_back(std::to_string(machine->id()));
            row.columns.push_back(fmt::format("\"{}\"", machine->name()));
            row.columns.emplace_back("MACH");
            table.rows.push_back(row);
        }

        table.rows.push_back({{fmt::format("{} registered machines", machines.size())}});

        context.result.add_data(
                "command_result",
                {{"data", table}});

        return true;
    }

    bool environment::on_delete_machine(
            const command_handler_context_t& context) {
        return hardware::registry::instance()->remove_machine(
                context.result,
                boost::get<core::string_literal_t>(context.params["name"].front()).value);
    }

    // ----------------------------------------------------------
    // editor commands
    // ----------------------------------------------------------
    bool environment::on_read_text(
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        auto value = boost::get<core::string_literal_t>(context.params["path"].front()).value;
        if (!is_regular_file(value)) {
            context.result.add_message(
                    "C021",
                    fmt::format("invalid path: {}", value),
                    true);
            return false;
        }

        context.result.add_data("command_action", {
                {"action", std::string("read_text")},
                {"name", value}
        });

        return true;
    }

    bool environment::on_goto_line(
            const command_handler_context_t& context) {
        auto line_number = static_cast<uint32_t>(boost::get<core::numeric_literal_t>(
                context.params["line"].front()).value);
        context.result.add_data("command_action", {
                {"action", std::string("goto_line")},
                {"line_number", line_number}
        });

        return true;
    }

    bool environment::on_find_text(
            const command_handler_context_t& context) {
        context.result.add_data("command_action", {
                {"action", std::string("find_text")},
                {"needle", boost::get<core::string_literal_t>(context.params["needle"].front()).value}
        });

        return true;
    }

    bool environment::on_write_text(
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        std::string value {"(default)"};
        if (!context.root->children.empty()) {
            value = boost::get<core::string_literal_t>(context.params["path"].front()).value;
            if (!is_regular_file(value)) {
                context.result.add_message(
                        "C022",
                        fmt::format("invalid path: {}", value),
                        true);
                return false;
            }
        }

        context.result.add_data("command_action", {
                {"action", std::string("write_text")},
                {"name", value}
        });

        return true;
    }

    bool environment::on_tile_editor(
            const command_handler_context_t& context) {
        context.result.add_data(
                "command_action",
                {{"action", std::string("edit_tiles")}});
        return true;
    }

    bool environment::on_module_editor(
            const command_handler_context_t& context) {
        context.result.add_data(
                "command_action",
                {{"action", std::string("edit_music")}});
        return true;
    }

    bool environment::on_source_editor(
            const command_handler_context_t& context) {
        auto path = boost::get<core::string_literal_t>(context.params["path"].front()).value;
        context.result.add_data(
                "command_action",
                {
                        {"action", std::string("edit_source")},
                        {"path", project::find_project_root().append(path).string()}
                });
        return true;
    }

    bool environment::on_sprite_editor(
            const command_handler_context_t& context) {
        context.result.add_data(
                "command_action",
                {{"action", std::string("edit_sprites")}});
        return true;
    }

    bool environment::on_sample_editor(
            const command_handler_context_t& context) {
        context.result.add_data(
                "command_action",
                {{"action", std::string("edit_sounds")}});
        return true;
    }

    bool environment::on_register_editor(
            const command_handler_context_t& context) {
        context.result.add_data(
                "command_action",
                {{"action", std::string("edit_cpu")}});
        return true;
    }

    bool environment::on_background_editor(
            const command_handler_context_t& context) {
        context.result.add_data(
                "command_action",
                {{"action", std::string("edit_backgrounds")}});
        return true;
    }

    // ----------------------------------------------------------
    // environment commands
    // ----------------------------------------------------------
    bool environment::on_new_environment(
            const command_handler_context_t& context) {
        if (core::project::instance() == nullptr) {
            context.result.add_message(
                    "C032",
                    "no project is loaded; unable to create environment",
                    true);
            return false;
        }

        auto name = boost::get<core::string_literal_t>(context.params["name"].front()).value;

        fs::path environment_file_path(name);
        if (environment_file_path.has_parent_path()) {
            context.result.add_message(
                    "C032",
                    "environment name cannot contain a path",
                    true);
            return false;
        }

        environment_file_path = fs::current_path()
                .append(".ryu")
                .append(environment_file_path
                                .replace_extension(".env")
                                .string());

        if (!fs::exists(environment_file_path.parent_path())) {
            context.result.add_message(
                    "C031",
                    fmt::format(
                            "project folder does not exist: {}",
                            environment_file_path.parent_path().string()),
                    true);
            return false;
        }

        if (fs::exists(environment_file_path)) {
            context.result.add_message(
                    "C031",
                    fmt::format("environment already exists: {}", environment_file_path.string()),
                    true);
            return false;
        }

        core::project_file file(
                core::id_pool::instance()->allocate(),
                name,
                core::project_file_type::environment);
        core::project::instance()->add_file(file);
        file.create_stub_file(context.result, environment_file_path);
        core::project::instance()->save(context.result);

        return !context.result.is_failed();
    }

    bool environment::on_list_environments(
            const command_handler_context_t& context) {
        if (core::project::instance() == nullptr) {
            context.result.add_message(
                    "C032",
                    "no project is loaded; unable to list environments",
                    true);
            return false;
        }

        data_table_t table {};
        table.headers.push_back({"ID",                   5,  5});
        table.headers.push_back({"Path",                20, 50});
        table.headers.push_back({"Type",                 8,  8});
        table.footers.push_back({"Environment Count",   15, 20});

        auto project_files = core::project::instance()->files();
        for (const auto& file : project_files) {
            if (file.type() != project_file_type::environment)
                continue;
            data_table_row_t row {};
            row.columns.push_back(std::to_string(file.id()));
            row.columns.push_back(fmt::format("\"{}\"", file.path().string()));
            row.columns.push_back(boost::to_upper_copy<std::string>(core::project_file_type::type_to_code(file.type())));
            table.rows.push_back(row);
        }

        table.rows.push_back({{fmt::format("{} environments", table.rows.size())}});

        context.result.add_data(
                "command_result",
                {{"data", table}});

        return true;
    }

    bool environment::on_remove_environment(
            const command_handler_context_t& context) {
        if (core::project::instance() == nullptr) {
            context.result.add_message(
                    "C032",
                    "no project is loaded; unable to remove environment",
                    true);
            return false;
        }

        auto name = boost::get<core::string_literal_t>(context.params["name"].front()).value;
        auto file = core::project::instance()->find_file(name);
        if (file == nullptr) {
            context.result.add_message(
                    "C032",
                    fmt::format("no environment file exists: {}", name),
                    true);
            return false;
        }

        core::project::instance()->remove_file(file->id());
        core::project::instance()->save(context.result);

        fs::path environment_file_path(fs::current_path()
               .append(".ryu")
               .append(fs::path(name)
                               .replace_extension(".env")
                               .string()));

        if (fs::exists(environment_file_path)) {
            boost::system::error_code ec;
            if (!fs::remove(environment_file_path, ec)) {
                context.result.add_message(
                        "C007",
                        fmt::format("remove environment file failed: {}", ec.message()),
                        true);
                return false;
            }
        }

        return true;
    }

    bool environment::on_switch_environment(
            const command_handler_context_t& context) {
        if (core::project::instance() == nullptr) {
            context.result.add_message(
                    "C032",
                    "no project is loaded; unable to switch environment",
                    true);
            return false;
        }

        auto name = boost::get<core::string_literal_t>(context.params["name"].front()).value;
        auto file = core::project::instance()->find_file(name);
        if (file == nullptr) {
            context.result.add_message(
                    "C032",
                    fmt::format("no environment file exists: {}", name),
                    true);
            return false;
        }

        auto active_environment = core::project::instance()->active_environment();
        if (active_environment != nullptr) {

        }

        core::project::instance()->active_environment(file);
        core::project::instance()->save(context.result);

        return true;
    }

}