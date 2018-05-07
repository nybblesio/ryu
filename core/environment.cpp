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
#include <common/bytes.h>
#include <core/evaluator.h>
#include <core/core_types.h>
#include <hardware/machine.h>
#include <hardware/registry.h>
#include <boost/filesystem.hpp>
#include <core/command_parser.h>
#include <common/string_support.h>
#include <common/stream_support.h>
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
            core::command::types::drop_symbols,
            std::bind(&environment::on_drop_symbols, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::edit_environment,
            std::bind(&environment::on_edit_environment, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::apply_environment,
            std::bind(&environment::on_apply_environment, std::placeholders::_1, std::placeholders::_2)
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
            core::command::types::peek,
            std::bind(&environment::on_peek_memory, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::poke,
            std::bind(&environment::on_poke_memory, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::assemble,
            std::bind(&environment::on_assemble, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::target,
            std::bind(&environment::on_set_target, std::placeholders::_1, std::placeholders::_2)
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
            core::command::types::memory_map,
            std::bind(&environment::on_memory_map, std::placeholders::_1, std::placeholders::_2)
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
            core::command::types::actor_editor,
            std::bind(&environment::on_actor_editor, std::placeholders::_1, std::placeholders::_2)
        },
        {
            core::command::types::palette_editor,
            std::bind(&environment::on_palette_editor, std::placeholders::_1, std::placeholders::_2)
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
            uint32_t value,
            core::command_size_flags size,
            std::vector<std::string>& results) {

        std::string ascii;
        std::string hex_fmt_spc,
                decimal_fmt_spec,
                ascii_fmt_spec,
                binary_fmt_spec;

        auto byte_count = 0;
        switch (size) {
            case core::command_size_flags::byte:
                byte_count = 1;
                hex_fmt_spc = "${0:02x}";
                decimal_fmt_spec = "{0:>3}";
                ascii_fmt_spec = "\"{0:>1}\"";
                binary_fmt_spec = "%{0:08b}";
                break;
            case core::command_size_flags::word:
                byte_count = 2;
                hex_fmt_spc = "${0:04x}";
                decimal_fmt_spec = "{0:>5}";
                ascii_fmt_spec = "\"{0:>2}\"";
                binary_fmt_spec = "%{0:016b}";
                break;
            case core::command_size_flags::none:
            case core::command_size_flags::dword:
                byte_count = 4;
                hex_fmt_spc = "${0:08x}";
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

        results.push_back(fmt::format(hex_fmt_spc, value));
        if ((value & 0b10000000000000000000000000000000) != 0)
            results.push_back(fmt::format(decimal_fmt_spec, static_cast<int32_t>(value)));
        else
            results.push_back(fmt::format(decimal_fmt_spec, value));
        results.push_back(fmt::format(ascii_fmt_spec, ascii));
        results.push_back(fmt::format(binary_fmt_spec, value));
    }

    environment::environment() : _assembler(std::make_unique<core::assembler>()),
                                 _evaluator(std::make_unique<core::evaluator>(_assembler.get())),
                                 _symbol_table(std::make_unique<core::symbol_table>()),
                                 _command_parser(std::make_unique<core::command_parser>()) {
        _evaluator->symbol_table(_symbol_table.get());
        _assembler->symbol_table(_symbol_table.get());
        _command_parser->symbol_table(_symbol_table.get());
    }

    environment::~environment() {
    }

    bool environment::execute(
            core::result& result,
            const std::string& line) {
        using namespace boost::filesystem;

        parser_input_t input{line};
        auto root = _command_parser->parse(input);
        if (root == nullptr || _command_parser->result().is_failed()) {
            for (auto& msg : _command_parser->result().messages())
                result.add_message(msg.code(), msg.message(), msg.is_error());
            result.fail();
            return false;
        }

        auto command = root->command_type();
        auto statement_node = root->lhs;

        core::command_parameter_dict params;
        if (statement_node->rhs->children.empty()) {
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
            size_t idx = 0;
            for (const auto& param_spec : command.spec.params) {
                std::vector<core::variant_t>* values = nullptr;

                if (param_spec.required) {
                    if (idx >= statement_node->rhs->children.size()) {
                        result.add_message(
                                "C801",
                                fmt::format("the parameter <italic>'{}'<> is required.", param_spec.name),
                                true);
                        result.fail();
                        break;
                    }
                }

                if (idx >= statement_node->rhs->children.size())
                    break;

                auto it = params.find(param_spec.name);
                if (it == params.end()) {
                    auto range = params.insert(std::make_pair(param_spec.name, std::vector<core::variant_t>{}));
                    it = range.first;
                }

                values = &it->second;

                auto value = param_spec.evaluate ?
                             _evaluator->evaluate(result, statement_node->rhs->children[idx]) :
                             statement_node->rhs->children[idx]->value;

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
                    while (idx < statement_node->rhs->children.size()) {
                        value = param_spec.evaluate ?
                                _evaluator->evaluate(result, statement_node->rhs->children[idx++]) :
                                statement_node->rhs->children[idx]->value;
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

        if (!root->rhs->children.empty()) {
            auto redirection_node = root->rhs->children[0];
            auto redirection_symbol = redirection_node->lhs->string_literal_type();
            if (redirection_symbol == "|") {
                result.add_data("pipe_to_more", {{"enabled", true}});
            }
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

    void environment::format_data_bytes(
            std::stringstream& stream,
            uint32_t address,
            const byte_list& bytes) {
        stream << fmt::format("{:08x}: ", address);
        for (auto byte_value : bytes)
            stream << fmt::format("{:02x}", byte_value);
        stream << "\n";
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

    bool environment::assemble(
            core::result& result,
            core::project_file* file) {
        if (!file->should_assemble())
            return false;

        std::stringstream source;
        if (!file->read_text(result, source))
            return false;

        auto source_text = source.str();
        return _assembler->assemble(result, parser_input_t {source_text});
    }

    bool environment::apply_environment(
            core::result& result,
            core::project_file* file) {
        if (!has_valid_project(result))
            return false;

        _symbol_table->clear();

        if (file != nullptr) {
            result.add_message(
                    "E001",
                    fmt::format("Apply Environment: <bold>{}<>", file->name()));
            result.add_message(
                    "E001",
                    fmt::format("Assemble: \n  <italic>{}<>\n", file->full_path()));
            if (assemble(result, file)) {
                name(file->name());
            }

            auto listing_table = _assembler->listing().table();

            if (listing_table.rows.size() > 2) {
                result.add_data(
                        "command_result",
                        {
                                {"listing", listing_table},
                                {"symbols", create_symbol_table()}
                        });
            }
        }

        return !result.is_failed();
    }

    bool environment::assemble(core::result& result) {
        if (!has_valid_project_and_machine(result))
            return false;

        auto files = core::project::instance()->files();
        for (const auto& file : files) {
            assemble(result, file.get());
        }

        auto listing_table = _assembler->listing().table();

        if (listing_table.rows.size() > 2) {
            result.add_data(
                    "command_result",
                    {
                        {"listing", listing_table},
                        {"symbols", create_symbol_table()}
                    });
        }

        return !result.is_failed();
    }

    // ----------------------------------------------------------
    // system commands
    // ----------------------------------------------------------
    bool environment::on_quit(
            const command_handler_context_t& context) {
        context.result.add_data(
                "command_action",
                {{"action", core::system_commands::quit}});
        return true;
    }

    bool environment::on_help(
            const command_handler_context_t& context) {
        using format_options = core::data_table_column_t::format_options;

        auto build_command_spec = [](std::stringstream& stream, const command_spec_t& cmd_spec) {
            stream << cmd_spec.command_name;

            if (cmd_spec.valid_sizes != core::command_size_flags::none) {
                stream << "<italic>[.";

                if ((cmd_spec.valid_sizes & core::command_size_flags::byte) != 0) {
                    stream << "b";
                }

                if ((cmd_spec.valid_sizes & core::command_size_flags::word) != 0) {
                    stream << "|w";
                }

                if ((cmd_spec.valid_sizes & core::command_size_flags::dword) != 0) {
                    stream << "|dw";
                }

                if ((cmd_spec.valid_sizes & core::command_size_flags::qword) != 0) {
                    stream << "|qw";
                }

                stream << "]<>";
            }

            if (!cmd_spec.params.empty()) {
                stream << " ";
                for (size_t i = 0; i < cmd_spec.params.size(); i++) {
                    auto param_spec = cmd_spec.params[i];
                    if (param_spec.required) {
                        stream << "<bold>" << param_spec.name << "<>";
                    } else {
                        stream << "<italic>[" << param_spec.name << "]<>";
                    }
                    if (i < cmd_spec.params.size() - 1)
                        stream << " ";
                }
            }
        };

        const auto commands = core::command_parser::command_catalog();

        auto command_name = context.get_parameter<core::string_literal_t>("cmd");
        if (!command_name.empty()) {
            auto command_it = commands.find(command_name);
            if (command_it != commands.end()) {
                std::stringstream stream;
                auto cmd_spec = command_it->second;
                cmd_spec.command_name = command_it->first;
                build_command_spec(stream, cmd_spec);

                context.result.add_message("H001", "\n<rev> Command Syntax <>");
                context.result.add_message("H001", stream.str());
                context.result.add_message("H001", "\n<rev> Category       <>");
                context.result.add_message("H001", cmd_spec.category);
                context.result.add_message("H001", "\n<rev> Overview       <>");
                context.result.add_message("H001", cmd_spec.help);
                context.result.add_message("H001", "\n<rev> Details        <>");
                context.result.add_message("H001", cmd_spec.full_help);
            }
        } else {
            data_table_t table{};
            table.line_spacing = 1;
            table.headers.push_back({
                    "Category",
                    20,
                    20,
                    alignment::horizontal::left,
                    1,
                    format_options::style_codes
            });
            table.headers.push_back({
                    "Command",
                    35,
                    35,
                    alignment::horizontal::left,
                    1,
                    format_options::style_codes
            });
            table.headers.push_back({
                    "Help",
                    60,
                    60,
                    alignment::horizontal::left,
                    1,
                    format_options::style_codes | format_options::word_wrap
            });
            table.footers.push_back({"Command Count", 15, 20});

            std::map<std::string, std::shared_ptr<command_spec_list>> categorized_commands;

            for (auto c : commands) {
                std::shared_ptr<command_spec_list> spec_list = nullptr;

                auto spec_it = categorized_commands.find(c.second.category);
                if (spec_it == categorized_commands.end()) {
                    spec_list = std::make_shared<command_spec_list>();
                    categorized_commands.insert(std::make_pair(c.second.category, spec_list));
                } else {
                    spec_list = spec_it->second;
                }

                c.second.command_name = c.first;
                spec_list->push_back(c.second);
            }

            for (const auto c : categorized_commands) {
                auto sorted_list = *(c.second);
                std::sort(
                        sorted_list.begin(),
                        sorted_list.end(),
                        [](const command_spec_t& l, const command_spec_t& r) {
                            return l.category < r.category && l.sequence < r.sequence;
                        });

                std::string category_name;

                for (const auto& cmd_spec : sorted_list) {
                    data_table_row_t row{};

                    if (category_name != cmd_spec.category) {
                        category_name = cmd_spec.category;
                        row.columns.push_back(category_name);
                    } else {
                        row.columns.emplace_back("");
                    }

                    std::stringstream stream;
                    build_command_spec(stream, cmd_spec);

                    row.columns.push_back(stream.str());
                    row.columns.push_back(cmd_spec.help);

                    table.rows.push_back(row);
                }
            }

            table.rows.push_back({
                 {fmt::format("{} available commands", commands.size())}
            });

            context.result.add_data("command_result", {{"data", table}});
        }

        return true;
    }

    bool environment::on_clear(
            const command_handler_context_t& context) {
        context.result.add_data(
                "command_action",
                {{"action", core::system_commands::clear}});
        return true;
    }

    bool environment::on_open_editor(
            const command_handler_context_t& context) {
        auto name = context.get_parameter<core::string_literal_t>("name");
        auto type = context.get_parameter<core::identifier_t>("type");

        if (project_file_type::does_type_require_project(type)
        && !has_valid_project(context.result))
            return false;

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

    bool environment::on_set_target(
            const command_handler_context_t& context) {
        if (!has_valid_project_and_machine(context.result))
            return false;
        auto name = context.get_parameter<string_literal_t>("name");
        return _assembler->load_target(context.result, name);
    }

    bool environment::on_add_symbol(
            const command_handler_context_t& context) {
        auto statement_node = context.root->lhs;
        if (statement_node->rhs->children.size() == 2) {
            auto identifier = context.get_parameter<core::identifier_t>("name");
            _symbol_table->put(identifier, statement_node->rhs->children[1]);
        } else {
            context.result.add_message(
                "C007",
                "set command requires two parameters",
                true);
        }

        return !context.result.is_failed();
    }

    bool environment::on_drop_symbols(
            const command_handler_context_t& context) {
        _symbol_table->clear();
        context.result.add_message("C006", "Symbol table cleared.");
        return !context.result.is_failed();
    }

    bool environment::on_edit_environment(
            const command_handler_context_t& context) {
        if (!has_valid_project(context.result))
            return false;

        core::project_file* active_environment = nullptr;

        auto name = context.get_parameter<core::string_literal_t>("name");
        if (name.empty()) {
            active_environment = core::project::instance()->active_environment();
        } else {
            active_environment = core::project::instance()->find_file(name);
        }

        if (active_environment == nullptr) {
            context.result.add_message(
                    "C032",
                    fmt::format("no environment file exists: {}", name),
                    true);
            return false;
        }

        context.result.add_data("command_action", {
                {"action", project_file_type::code_to_action("text")},
                {"name", active_environment->full_path().string()}
        });

        return false;
    }

    bool environment::on_disassemble(
            const command_handler_context_t& context) {
        if (!has_valid_project_machine_and_target(context.result))
            return false;

        return true;
    }

    bool environment::on_remove_symbol(
            const command_handler_context_t& context) {
        auto identifier = context.get_parameter<core::identifier_t>("name");
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

        const auto& values = context.get_variadic_parameters();
        for (const auto& param : values) {
            switch (param.which()) {
                case core::variant::types::char_literal: {
                    std::vector<std::string> results;
                    format_numeric_conversion(
                            boost::get<core::char_literal_t>(param),
                            context.command.size,
                            results);
                    conversion_table.rows.push_back({results});
                    break;
                }
                case core::variant::types::numeric_literal: {
                    std::vector<std::string> results;
                    format_numeric_conversion(
                            boost::get<core::numeric_literal_t>(param),
                            context.command.size,
                            results);
                    conversion_table.rows.push_back({results});
                    break;
                }
                case core::variant::types::string_literal: {
                    auto value = static_cast<std::string>(boost::get<core::string_literal_t>(param));
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
                    std::vector<std::string> results;
                    format_numeric_conversion(
                            boost::get<core::boolean_literal_t>(param),
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
    bool environment::on_peek_memory(
            const command_handler_context_t& context) {
        if (!has_valid_project_machine_and_target(context.result))
            return false;

        auto addr = context.get_parameter<core::numeric_literal_t>("addr");
        _assembler->location_counter(addr);

        std::vector<uint8_t> data_bytes {};
        switch (context.command.size) {
            case command_size_flags::none:
            case command_size_flags::byte:
                data_bytes = _assembler->read_data(directive_t::data_sizes::byte);
                break;
            case command_size_flags::word:
                data_bytes = _assembler->read_data(directive_t::data_sizes::word);
                break;
            case command_size_flags::dword:
                data_bytes = _assembler->read_data(directive_t::data_sizes::dword);
                break;
            default:
                break;
        }

        std::stringstream stream;
        format_data_bytes(stream, addr, data_bytes);

        context.result.add_data("command_result", {{"data", stream.str()}});

        return !context.result.is_failed();
    }

    bool environment::on_poke_memory(
            const command_handler_context_t& context) {
        if (!has_valid_project_machine_and_target(context.result))
            return false;

        auto addr = context.get_parameter<core::numeric_literal_t>("addr");
        _assembler->location_counter(addr);

        const auto& values = context.get_variadic_parameters();
        for (const auto& param : values) {
            uint32_t data_value = 0;

            switch (param.which()) {
                case core::variant::types::char_literal: {
                    data_value = boost::get<core::char_literal_t>(param);
                    break;
                }
                case core::variant::types::numeric_literal: {
                    data_value = boost::get<core::numeric_literal_t>(param);
                    break;
                }
                case core::variant::types::string_literal: {
                    auto string_value = boost::get<core::string_literal_t>(param);
                    break;
                }
                case core::variant::types::boolean_literal: {
                    data_value = boost::get<core::boolean_literal_t>(param);
                    break;
                }
                default: {
                    break;
                }
            }

            switch (context.command.size) {
                case command_size_flags::none:
                case command_size_flags::byte:
                    _assembler->write_data(directive_t::data_sizes::byte, data_value);
                    break;
                case command_size_flags::word:
                    _assembler->write_data(directive_t::data_sizes::word, data_value);
                    break;
                case command_size_flags::dword:
                    _assembler->write_data(directive_t::data_sizes::dword, data_value);
                    break;
                default:
                    break;
            }
        }

        return !context.result.is_failed();
    }

    bool environment::on_copy_memory(
            const command_handler_context_t& context) {
        if (!has_valid_project_machine_and_target(context.result))
            return false;

        auto dest_address = context.get_parameter<core::numeric_literal_t>("dest");
        auto src_address = context.get_parameter<core::numeric_literal_t>("src");
        auto count = context.get_parameter<core::numeric_literal_t>("count");
        auto byte_count = command_t::size_to_byte_count(context.command.size);

        for (size_t i = 0; i < count * byte_count; i++) {
            _assembler->location_counter(src_address + i);
            auto data_bytes = _assembler->read_data(directive_t::data_sizes::byte);
            _assembler->location_counter(dest_address + i);
            _assembler->write_data(directive_t::data_sizes::byte, data_bytes.front());
        }

        return !context.result.is_failed();
    }

    bool environment::on_fill_memory(
            const command_handler_context_t& context) {
        if (!has_valid_project_machine_and_target(context.result))
            return false;

        auto addr = context.get_parameter<core::numeric_literal_t>("addr");
        _assembler->location_counter(addr);

        auto count = context.get_parameter<core::numeric_literal_t>("count");
        auto value = context.get_parameter<core::numeric_literal_t>("value");
        auto byte_count = command_t::size_to_byte_count(context.command.size);

        for (size_t i = 0; i < count / byte_count; i++) {
            switch (context.command.size) {
                case command_size_flags::none:
                case command_size_flags::byte:
                    _assembler->write_data(directive_t::data_sizes::byte, value);
                    break;
                case command_size_flags::word:
                    _assembler->write_data(directive_t::data_sizes::word, value);
                    break;
                case command_size_flags::dword:
                    _assembler->write_data(directive_t::data_sizes::dword, value);
                    break;
                default:
                    break;
            }
        }

        return !context.result.is_failed();
    }

    bool environment::on_dump_memory(
            const command_handler_context_t& context) {
        if (!has_valid_project_machine_and_target(context.result))
            return false;

        auto addr = context.get_parameter<core::numeric_literal_t>("addr");
        auto bytes = context.get_parameter<core::numeric_literal_t>("bytes");

        auto byte_count = std::max<uint32_t>(bytes.value, 128);
        auto buffer = new uint8_t[byte_count];

        auto machine = core::project::instance()->machine();
        if (machine == nullptr) {
            context.result.add_message(
                    "C033",
                    "no machine active on loaded project; dump memory failed",
                    true);
            return false;
        }

        for (size_t i = 0; i < byte_count; i++) {
            buffer[i] = machine
                    ->mapper()
                    ->read_byte(static_cast<uint32_t>(addr + i));
        }

        context.result.add_data(
            "command_result",
            {
                {
                    "data",
                    core::hex_formatter::dump_to_table(buffer, byte_count, 16)
                }
            });

        delete[] buffer;

        return true;
    }

    bool environment::on_search_memory(
            const command_handler_context_t& context) {
        if (!has_valid_project_machine_and_target(context.result))
            return false;

        auto addr = context.get_parameter<core::numeric_literal_t>("addr").value;
        auto count = context.get_parameter<core::numeric_literal_t>("count");
        auto byte_count = command_t::size_to_byte_count(context.command.size);
        auto value = context.get_parameter<core::numeric_literal_t>("value").value;

        address_list matched_addresses {};

        for (size_t i = 0; i < count; i++) {
            _assembler->location_counter(addr);
            switch (context.command.size) {
                case command_size_flags::none:
                case command_size_flags::byte: {
                    auto data_bytes = _assembler->read_data(directive_t::data_sizes::byte);
                    if (value == data_bytes.front()) {
                        matched_addresses.push_back(_assembler->location_counter());
                    }
                    break;
                }
                case command_size_flags::word: {
                    auto data_bytes = _assembler->read_data(directive_t::data_sizes::word);
                    auto endian_correct_value = static_cast<uint16_t>(value);
                    if (is_platform_little_endian()) {
                        endian_correct_value = endian_swap_word(endian_correct_value);
                    }
                    auto memory_word = *(reinterpret_cast<uint16_t*>(data_bytes.data()));
                    if (memory_word == endian_correct_value)
                        matched_addresses.push_back(_assembler->location_counter());
                    break;
                }
                case command_size_flags::dword: {
                    auto data_bytes = _assembler->read_data(directive_t::data_sizes::dword);
                    auto endian_correct_value = value;
                    if (is_platform_little_endian()) {
                        endian_correct_value = endian_swap_dword(endian_correct_value);
                    }
                    auto memory_dword = *(reinterpret_cast<uint32_t*>(data_bytes.data()));
                    if (memory_dword == endian_correct_value)
                        matched_addresses.push_back(_assembler->location_counter());
                    break;
                }
                default:
                    break;
            }
            addr += byte_count;
        }

        data_table_t table {};
        table.headers.push_back({"Address", 8, 8});
        for (auto i = 0; i < 8; i++)
            table.headers.push_back({fmt::format("{:02x}", i), 2, 2});
        for (auto i = 0; i < 8; i++) {
            table.headers.push_back({
                    "",
                    1,
                    1,
                    core::alignment::horizontal::types::left,
                    0
            });
        }
        table.footers.push_back({"Locations Found Count", 20, 20});

        for (auto matched_address : matched_addresses) {
            data_table_row_t row {};

            row.columns.push_back(fmt::format("{:08x}", matched_address));

            byte_list data_bytes {};
            for (auto i = 0; i < 8; i++) {
                _assembler->location_counter(matched_address + i);
                data_bytes.push_back(_assembler->read_data(directive_t::data_sizes::byte).front());
            }

            for (auto i = 0; i < 8; i++) {
                row.columns.push_back(fmt::format("{:02x}", data_bytes[i]));
            }

            for (auto i = 0; i < 8; i++) {
                row.columns.push_back(fmt::format(
                        "{}",
                        (char) (isprint(data_bytes[i]) ? data_bytes[i] : '.')));
            }

            table.rows.push_back(row);
        }

        table.rows.push_back({{fmt::format(
                "{} locations matched",
                matched_addresses.size())}});

        context.result.add_data("command_result", {{"data", table}});

        return !context.result.is_failed();
    }

    bool environment::on_memory_editor(
            const command_handler_context_t& context) {
        if (!has_valid_project_machine_and_target(context.result))
            return false;

        auto addr = context.get_parameter<core::numeric_literal_t>("addr");

        context.result.add_data(
                "command_action",
                {
                    {"action", core::system_commands::edit_memory},
                    {"addr", addr.value}
                });
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
        if (!has_valid_project_machine_and_target(context.result))
            return false;

        auto project = core::project::instance();
        fs::path path(context.get_parameter<core::string_literal_t>("path").value);
        if (!path.is_absolute())
            path = project->path().append(path.string());

        auto start = context.get_parameter<core::numeric_literal_t>("start");
        auto end = context.get_parameter<core::numeric_literal_t>("end");

        _assembler->location_counter(start);

        std::stringstream stream;
        if (!ryu::read_binary(context.result, path, stream))
            return false;

        byte_list data_bytes {};
        _assembler->load_binary_to_location_counter(
                context.result,
                data_bytes,
                stream,
                end);

        context.result.add_data(
                "command_result",
                {
                    {
                        "data",
                        core::hex_formatter::dump_to_table(
                                data_bytes.data(),
                                data_bytes.size(),
                                16)
                    }
                });

        return !context.result.is_failed();
    }

    bool environment::on_write_memory_to_binary(
            const command_handler_context_t& context) {
        if (!has_valid_project_machine_and_target(context.result))
            return false;

        auto project = core::project::instance();
        fs::path path(context.get_parameter<core::string_literal_t>("path").value);
        if (!path.is_absolute())
            path = project->path().append(path.string());

        auto start = context.get_parameter<core::numeric_literal_t>("start");
        auto end = context.get_parameter<core::numeric_literal_t>("end");

        std::stringstream stream;
        _assembler->location_counter(start);
        _assembler->read_location_counter_to_binary(
                context.result,
                stream,
                end);
        if (!ryu::write_binary(context.result, path, stream))
            return false;

        context.result.add_message(
                "M001",
                fmt::format("{} bytes written to {}", _assembler->location_counter() - start, path));

        return !context.result.is_failed();
    }

    // ----------------------------------------------------------
    // file system commands
    // ----------------------------------------------------------
    bool environment::on_move_file(
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        auto src = context.get_parameter<core::string_literal_t>("src");
        auto dest = context.get_parameter<core::string_literal_t>("dest");

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
        auto path = context.get_parameter<core::string_literal_t>("path");
        if (!is_directory(path) && !is_regular_file(path)) {
            context.result.add_message(
                    "C008",
                    fmt::format("invalid path: {}", path), true);
        } else {
            boost::system::error_code ec;

            if (remove(path, ec)) {
                context.result.add_message(
                        "C008",
                        fmt::format("removal success: {}", path));
            } else {
                context.result.add_message(
                        "C008",
                        fmt::format("removal failed: {}", ec.message()),
                        true);
            }
        }

        return !context.result.is_failed();
    }

    bool environment::on_make_directory(
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        auto path = context.get_parameter<core::string_literal_t>("path");
        boost::system::error_code ec;

        if (!create_directory(path, ec)) {
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

        auto path = context.get_parameter<core::string_literal_t>("path");
        if (!is_directory(path)) {
            context.result.add_message(
                    "C007",
                    fmt::format("invalid path: {}", path),
                    true);
            return false;
        }

        current_path(path);

        context.result.add_data(
                "command_action",
                {
                    {"action", core::system_commands::update_working_directory},
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
                context.get_parameter<core::string_literal_t>("path"));
    }

    bool environment::on_edit_project(
            const command_handler_context_t& context) {
        if (!has_valid_project(context.result))
            return false;

        context.result.add_data(
                "command_action",
                {{"action", core::system_commands::edit_project}});
        return true;
    }

    bool environment::on_load_project(
            const command_handler_context_t& context) {
        auto path = context.get_parameter<core::string_literal_t>("path");
        if (core::project::load(context.result, path)) {
            apply_environment(
                    context.result,
                    core::project::instance()->active_environment());
        }

        return !context.result.is_failed();
    }

    bool environment::on_save_project(
            const command_handler_context_t& context) {
        if (!has_valid_project(context.result))
            return false;
        return core::project::instance()->save(context.result);
    }

    bool environment::on_close_project(
            const command_handler_context_t& context) {
        if (!has_valid_project(context.result))
            return false;
        auto result = core::project::close(context.result);
        if (result)
            _symbol_table->clear();
        return result;
    }

    bool environment::on_clone_project(
            const command_handler_context_t& context) {
        if (!has_valid_project(context.result))
            return false;
        return core::project::clone(
                context.result,
                core::project::instance()->path(),
                context.get_parameter<core::string_literal_t>("path").value);
    }

    bool environment::on_save_project_file(
            const command_handler_context_t& context) {
        if (!has_valid_project(context.result))
            return false;

        context.result.add_data(
                "command_action",
                {{"action", core::system_commands::save_project_file}});

        return true;
    }

    bool environment::on_new_project_file(
            const command_handler_context_t& context) {
        if (!has_valid_project(context.result))
            return false;

        auto type = context.get_parameter<core::identifier_t>("type");
        if (type.empty())
            type.value = "TEXT";
        auto path = context.get_parameter<core::string_literal_t>("path");

        auto project = core::project::instance();
        auto file = std::make_shared<core::project_file>(
                core::id_pool::instance()->allocate(),
                project,
                path,
                core::project_file_type::code_to_type(type));
        project->add_file(file);
        file->create_stub_file(context.result, path);

        return !context.result.is_failed();
    }

    bool environment::on_remove_project_file(
            const command_handler_context_t& context) {
        if (!has_valid_project(context.result))
            return false;

        auto path = context.get_parameter<core::string_literal_t>("path");
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
        if (!has_valid_project(context.result))
            return false;

        data_table_t table {};
        table.headers.push_back({"ID",           5,  5});
        table.headers.push_back({"Path",        20, 50});
        table.headers.push_back({"Type",         8,  8});
        table.footers.push_back({"File Count",  15,  20});

        auto project_files = core::project::instance()->files();
        for (const auto& file : project_files) {
            if (file->type() == project_file_type::environment)
                continue;
            data_table_row_t row {};
            row.columns.push_back(std::to_string(file->id()));
            row.columns.push_back(fmt::format("\"{}\"", file->path().string()));
            auto code = core::project_file_type::type_to_code(file->type());
            to_upper(code);
            row.columns.push_back(code);
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
    bool environment::on_memory_map(
            const command_handler_context_t& context) {
        using format_options = data_table_column_t::format_options;

        if (!has_valid_project_and_machine(context.result))
            return false;

        data_table_t table {};
        table.headers.push_back({"Component Name",   20,  20});
        table.headers.push_back({"Start",             8,   8});
        table.headers.push_back({
                "",
                1,
                1,
                alignment::horizontal::types::left,
                1
        });
        table.headers.push_back({"End",               8,   8});
        table.headers.push_back({"Name",             10,  10});
        table.headers.push_back({"Offset",            8,   8});
        table.headers.push_back({"Size",              4,   4});
        table.headers.push_back({
                "Details",
                55,
                55,
                alignment::horizontal::types::left,
                1,
                format_options::word_wrap | format_options::style_codes
        });
        table.footers.push_back({"Component Count",  15,  20});

        auto mapper = core::project::instance()->machine()->mapper();
        auto mapper_components = mapper->components();

        for (auto component_interval : mapper_components) {
            auto map_entries = component_interval.value->memory_map().entries();

            once_value_t component_name;
            once_value_t component_start;
            once_value_t component_end;

            for (auto entry_interval : map_entries) {
                data_table_row_t row {};

                component_name.add_once_column(row, component_interval.value->component()->name());
                component_end.add_once_column(row, fmt::format("{:<8x}", component_interval.stop));
                row.columns.emplace_back("-");
                component_start.add_once_column(row, fmt::format("{:<8x}", component_interval.start));

                row.columns.push_back(entry_interval.value->name());
                row.columns.push_back(fmt::format("{:<8x}", entry_interval.start));
                row.columns.push_back(fmt::format("{:<4x}", entry_interval.stop - entry_interval.start));
                row.columns.push_back(entry_interval.value->description());
                table.rows.push_back(row);
            }

            data_table_row_t row {};
            row.columns.emplace_back("");
            row.columns.emplace_back("");
            row.columns.emplace_back("");
            row.columns.emplace_back("");
            row.columns.emplace_back("");
            row.columns.emplace_back("");
            row.columns.emplace_back("");
            table.rows.push_back(row);
        }

        table.rows.push_back({{fmt::format(
                "{} components mapped",
                table.rows.size())}});

        context.result.add_data(
                "command_result",
                {{"data", table}});

        return true;
    }

    bool environment::on_use_machine(
            const command_handler_context_t& context) {
        if (!has_valid_project(context.result))
            return false;

        auto machine_name = context.get_parameter<core::string_literal_t>("name");
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
        auto action = core::system_commands::edit_machine;
        auto name = context.get_parameter<core::string_literal_t>("name");
        if (name.empty())
            action = core::system_commands::list_machine;
        context.result.add_data(
                "command_action",
                {
                    {"action", action},
                    {"name", name}
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
                context.get_parameter<core::string_literal_t>("name"));
    }

    // ----------------------------------------------------------
    // editor commands
    // ----------------------------------------------------------
    bool environment::on_read_text(
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        auto path = context.get_parameter<core::string_literal_t>("path");
        if (!is_regular_file(path)) {
            context.result.add_message(
                    "C021",
                    fmt::format("invalid path: {}", path),
                    true);
            return false;
        }

        context.result.add_data("command_action", {
                {"action", core::system_commands::read_text},
                {"name", path}
        });

        return true;
    }

    bool environment::on_goto_line(
            const command_handler_context_t& context) {
        auto line_number = context.get_parameter<core::numeric_literal_t>("line");
        context.result.add_data("command_action", {
                {"action", core::system_commands::goto_line},
                {"line_number", line_number}
        });

        return true;
    }

    bool environment::on_find_text(
            const command_handler_context_t& context) {
        context.result.add_data("command_action", {
                {"action", core::system_commands::find_text},
                {"needle", context.get_parameter<core::string_literal_t>("needle")}
        });

        return true;
    }

    bool environment::on_write_text(
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        auto path = context.get_parameter<core::string_literal_t>("path");
        if (!is_regular_file(path)) {
            context.result.add_message(
                    "C022",
                    fmt::format("invalid path: {}", path),
                    true);
            return false;
        }

        context.result.add_data("command_action", {
                {"action", core::system_commands::write_text},
                {"name", path}
        });

        return true;
    }

    bool environment::on_tile_editor(
            const command_handler_context_t& context) {
        if (!has_valid_project_and_machine(context.result))
            return false;

        context.result.add_data(
                "command_action",
                {{"action", core::system_commands::edit_tiles}});
        return true;
    }

    bool environment::on_module_editor(
            const command_handler_context_t& context) {
        if (!has_valid_project_and_machine(context.result))
            return false;

        context.result.add_data(
                "command_action",
                {{"action", core::system_commands::edit_music}});
        return true;
    }

    bool environment::on_source_editor(
            const command_handler_context_t& context) {
        if (!has_valid_project(context.result))
            return false;

        auto path = context.get_parameter<core::string_literal_t>("path");
        context.result.add_data(
                "command_action",
                {
                    {"action", core::system_commands::edit_source},
                    {"name", path}
                });
        return true;
    }

    bool environment::on_sprite_editor(
            const command_handler_context_t& context) {
        if (!has_valid_project_and_machine(context.result))
            return false;

        context.result.add_data(
                "command_action",
                {{"action", core::system_commands::edit_sprites}});
        return true;
    }

    bool environment::on_sample_editor(
            const command_handler_context_t& context) {
        if (!has_valid_project_and_machine(context.result))
            return false;

        context.result.add_data(
                "command_action",
                {{"action", core::system_commands::edit_sounds}});
        return true;
    }

    bool environment::on_register_editor(
            const command_handler_context_t& context) {
        if (!has_valid_project_machine_and_target(context.result))
            return false;

        context.result.add_data(
                "command_action",
                {{"action", core::system_commands::edit_cpu}});
        return true;
    }

    bool environment::on_background_editor(
            const command_handler_context_t& context) {
        if (!has_valid_project_and_machine(context.result))
            return false;
        context.result.add_data(
                "command_action",
                {{"action", core::system_commands::edit_backgrounds}});
        return true;
    }

    bool environment::on_actor_editor(
            const command_handler_context_t& context) {
        if (!has_valid_project_and_machine(context.result))
            return false;
        context.result.add_data(
            "command_action",
            {{"action", core::system_commands::edit_actor}});
        return false;
    }

    bool environment::on_palette_editor(
            const command_handler_context_t& context) {
        if (!has_valid_project_and_machine(context.result))
            return false;
        context.result.add_data(
            "command_action",
            {{"action", core::system_commands::edit_palette}});
        return false;
    }

    // ----------------------------------------------------------
    // environment commands
    // ----------------------------------------------------------
    bool environment::on_new_environment(
            const command_handler_context_t& context) {
        if (!has_valid_project(context.result))
            return false;

        auto name = context.get_parameter<core::string_literal_t>("name");

        fs::path environment_file_path(name.value);
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

        auto project = core::project::instance();
        auto file = std::make_shared<core::project_file>(
                core::id_pool::instance()->allocate(),
                project,
                name.value,
                core::project_file_type::environment);
        project->add_file(file);
        file->create_stub_file(context.result, environment_file_path);
        project->save(context.result);

        return !context.result.is_failed();
    }

    bool environment::on_apply_environment(
            const command_handler_context_t& context) {
        if (!has_valid_project(context.result))
            return false;
        return apply_environment(
                context.result,
                core::project::instance()->active_environment());
    }

    bool environment::on_list_environments(
            const command_handler_context_t& context) {
        if (!has_valid_project(context.result))
            return false;

        data_table_t table {};
        table.headers.push_back({"ID",                   5,  5});
        table.headers.push_back({"Path",                20, 50});
        table.headers.push_back({"Type",                 8,  8});
        table.footers.push_back({"Environment Count",   15, 20});

        auto project_files = core::project::instance()->files();
        for (const auto& file : project_files) {
            if (file->type() != project_file_type::environment)
                continue;
            data_table_row_t row {};
            row.columns.push_back(std::to_string(file->id()));
            row.columns.push_back(fmt::format("\"{}\"", file->path().string()));
            auto code = core::project_file_type::type_to_code(file->type());
            to_upper(code);
            row.columns.push_back(code);
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
        if (!has_valid_project(context.result))
            return false;

        auto name = context.get_parameter<core::string_literal_t>("name");
        auto file = core::project::instance()->find_file(name.value);
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
               .append(fs::path(static_cast<std::string>(name))
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
        core::project_file* active_environment = nullptr;

        auto name = context.get_parameter<core::string_literal_t>("name");

        if (!name.empty()) {
            active_environment = core::project::instance()->find_file(name);
            if (active_environment == nullptr) {
                context.result.add_message(
                        "C032",
                        fmt::format("no environment file exists: {}", name),
                        true);
                return false;
            }
        }

        core::project::instance()->active_environment(active_environment);
        if (apply_environment(context.result, active_environment)) {
            core::project::instance()->save(context.result);
        }

        return !context.result.is_failed();
    }

    bool environment::has_valid_project(core::result& result) {
        auto project = core::project::instance();
        if (project == nullptr) {
            result.add_message(
                    "C033",
                    "no project is loaded",
                    true);
            return false;
        }

        return true;
    }

    bool environment::has_valid_project_and_machine(core::result& result) {
        if (!has_valid_project(result))
            return false;

        if (core::project::instance()->machine() == nullptr) {
            result.add_message(
                    "C033",
                    "loaded project has no machine associated",
                    true);
            return false;
        }

        return true;
    }

    bool environment::has_valid_project_machine_and_target(core::result& result) {
        if (!has_valid_project_and_machine(result))
            return false;

        if (_assembler->target() == nullptr) {
            result.add_message(
                    "C033",
                    "no target component is set for the environment",
                    true);
            return false;
        }

        return true;
    }

}