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
#include "environment.h"
#include "text_formatter.h"

// TODO
//
// - refactor usage of core::result::add_message to be ::add_data or
//   something new that supports passing a display-agnostic table-like
//   structure back to the caller.
//
// - should we support quad-words as a display size
//

namespace ryu::core {

    // XXX: bug some values result in the ASCII conversion failing
    static void format_numeric_conversion(
            core::result& result,
            int32_t value,
            core::command_size_flags size) {
        std::string ascii;
        std::string fmt_spec;

        uint32_t mask = 0;
        auto byte_count = 0;
        switch (size) {
            case core::command_size_flags::byte:
                byte_count = 1;
                mask = 0xff;
                fmt_spec = "${0:02x} {1:>3} \"{2:>1}\" %{0:08b}";
                break;
            case core::command_size_flags::word:
                byte_count = 2;
                mask = 0xffff;
                fmt_spec = "${0:04x} {1:>5} \"{2:>2}\" %{0:016b}";
                break;
            case core::command_size_flags::none:
            case core::command_size_flags::dword:
                byte_count = 4;
                mask = 0xffffffff;
                fmt_spec = "${0:08x} {1:>10} \"{2:>4}\" %{0:032b}";
                break;
            default:
                // XXX: support for qword
                break;
        }

        auto* p = (char*) (&value) + (byte_count - 1);
        for (auto i = 0; i < byte_count; i++) {
            char c = *p--;
            if (c == 0)
                ascii += ".";
            else if (c == 10)
                ascii += "\\n";
            else
                ascii += c;
        }
        auto signed_value = value & mask;
        result.add_message(
                "C003",
                fmt::format(fmt_spec, signed_value, value, ascii));
    }

    bool environment::execute(
            core::result& result,
            const std::string& line) {
        using namespace boost::filesystem;

        static std::map<uint8_t, environment::command_handler_callable> command_handlers = {
            {core::command_types::quit,                   [&](core::result& result, const command_handler_context_t& context) { return on_quit(result, context); }},
            {core::command_types::help,                   [&](core::result& result, const command_handler_context_t& context) { return on_help(result, context); }},
            {core::command_types::clear,                  [&](core::result& result, const command_handler_context_t& context) { return on_clear(result, context); }},
            {core::command_types::add_symbol,             [&](core::result& result, const command_handler_context_t& context) { return on_add_symbol(result, context); }},
            {core::command_types::remove_symbol,          [&](core::result& result, const command_handler_context_t& context) { return on_remove_symbol(result, context); }},
            {core::command_types::show_symbol_table,      [&](core::result& result, const command_handler_context_t& context) { return on_show_symbol_table(result, context); }},
            {core::command_types::assemble,               [&](core::result& result, const command_handler_context_t& context) { return on_assemble(result, context); }},
            {core::command_types::evaluate,               [&](core::result& result, const command_handler_context_t& context) { return on_evaluate(result, context); }},
            {core::command_types::disassemble,            [&](core::result& result, const command_handler_context_t& context) { return on_disassemble(result, context); }},
            {core::command_types::dump_memory,            [&](core::result& result, const command_handler_context_t& context) { return on_dump_memory(result, context); }},
            {core::command_types::search_memory,          [&](core::result& result, const command_handler_context_t& context) { return on_search_memory(result, context); }},
            {core::command_types::fill_memory,            [&](core::result& result, const command_handler_context_t& context) { return on_fill_memory(result, context); }},
            {core::command_types::copy_memory,            [&](core::result& result, const command_handler_context_t& context) { return on_copy_memory(result, context); }},
            {core::command_types::jump_to_address,        [&](core::result& result, const command_handler_context_t& context) { return on_jump_to_address(result, context); }},
            {core::command_types::go_to_address,          [&](core::result& result, const command_handler_context_t& context) { return on_go_to_address(result, context); }},
            {core::command_types::register_editor,        [&](core::result& result, const command_handler_context_t& context) { return on_register_editor(result, context); }},
            {core::command_types::list_files,             [&](core::result& result, const command_handler_context_t& context) { return on_list_files(result, context); }},
            {core::command_types::remove_file,            [&](core::result& result, const command_handler_context_t& context) { return on_remove_file(result, context); }},
            {core::command_types::move_file,              [&](core::result& result, const command_handler_context_t& context) { return on_move_file(result, context); }},
            {core::command_types::make_directory,         [&](core::result& result, const command_handler_context_t& context) { return on_make_directory(result, context); }},
            {core::command_types::change_directory,       [&](core::result& result, const command_handler_context_t& context) { return on_change_directory(result, context); }},
            {core::command_types::print_working_directory,[&](core::result& result, const command_handler_context_t& context) { return on_print_working_directory(result, context); }},
            {core::command_types::new_project,            [&](core::result& result, const command_handler_context_t& context) { return on_new_project(result, context); }},
            {core::command_types::edit_project,           [&](core::result& result, const command_handler_context_t& context) { return on_edit_project(result, context); }},
            {core::command_types::load_project,           [&](core::result& result, const command_handler_context_t& context) { return on_load_project(result, context); }},
            {core::command_types::save_project,           [&](core::result& result, const command_handler_context_t& context) { return on_save_project(result, context); }},
            {core::command_types::close_project,          [&](core::result& result, const command_handler_context_t& context) { return on_close_project(result, context); }},
            {core::command_types::clone_project,          [&](core::result& result, const command_handler_context_t& context) { return on_clone_project(result, context); }},
            {core::command_types::list_project_files,     [&](core::result& result, const command_handler_context_t& context) { return on_list_project_files(result, context); }},
            {core::command_types::edit_machine,           [&](core::result& result, const command_handler_context_t& context) { return on_edit_machine(result, context); }},
            {core::command_types::list_machines,          [&](core::result& result, const command_handler_context_t& context) { return on_list_machines(result, context); }},
            {core::command_types::del_machine,            [&](core::result& result, const command_handler_context_t& context) { return on_del_machine(result, context); }},
            {core::command_types::use_machine,            [&](core::result& result, const command_handler_context_t& context) { return on_use_machine(result, context); }},
            {core::command_types::open_editor,            [&](core::result& result, const command_handler_context_t& context) { return on_open_editor(result, context); }},
            {core::command_types::source_editor,          [&](core::result& result, const command_handler_context_t& context) { return on_source_editor(result, context); }},
            {core::command_types::memory_editor,          [&](core::result& result, const command_handler_context_t& context) { return on_memory_editor(result, context); }},
            {core::command_types::sprite_editor,          [&](core::result& result, const command_handler_context_t& context) { return on_sprite_editor(result, context); }},
            {core::command_types::tile_editor,            [&](core::result& result, const command_handler_context_t& context) { return on_tile_editor(result, context); }},
            {core::command_types::background_editor,      [&](core::result& result, const command_handler_context_t& context) { return on_background_editor(result, context); }},
            {core::command_types::tracker,                [&](core::result& result, const command_handler_context_t& context) { return on_tracker(result, context); }},
            {core::command_types::sounds,                 [&](core::result& result, const command_handler_context_t& context) { return on_sounds(result, context); }},
            {core::command_types::read_binary_to_memory,  [&](core::result& result, const command_handler_context_t& context) { return on_read_binary_to_memory(result, context); }},
            {core::command_types::write_memory_to_binary, [&](core::result& result, const command_handler_context_t& context) { return on_write_memory_to_binary(result, context); }},
            {core::command_types::read_text,              [&](core::result& result, const command_handler_context_t& context) { return on_read_text(result, context); }},
            {core::command_types::write_text,             [&](core::result& result, const command_handler_context_t& context) { return on_write_text(result, context); }},
            {core::command_types::goto_line,              [&](core::result& result, const command_handler_context_t& context) { return on_goto_line(result, context); }},
            {core::command_types::find_text,              [&](core::result& result, const command_handler_context_t& context) { return on_find_text(result, context); }},
        };

        core::command_parser parser;
        parser.symbol_table(&_symbol_table);

        auto root = parser.parse(line);
        if (root == nullptr || parser.result().is_failed()) {
            for (auto& msg : parser.result().messages())
                result.add_message(msg.code(), msg.message(), msg.is_error());
            result.fail();
            return false;
        }

        auto command = boost::get<core::command_t>(root->value);
        core::evaluator evaluator;
        evaluator.symbol_table(&_symbol_table);

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
                auto it = params.find(param_spec.name);
                if (it == params.end()) {
                    auto range = params.insert(std::make_pair(param_spec.name, std::vector<core::variant_t>{}));
                    it = range.first;
                }
                values = &it->second;

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

        auto handler_it = command_handlers.find(command.spec.type);
        if (handler_it == command_handlers.end()) {
            result.add_message("C400", "Command not implemented.", true);
            return false;
        }

        return handler_it->second(
                result,
                command_handler_context_t {command, params, root});
    }

    core::symbol_table* environment::symbol_table() {
        return &_symbol_table;
    }

    bool environment::assemble(
            core::result& result,
            core::project& project) {
        return false;
    }

    bool environment::load(
            core::result& result,
            const boost::filesystem::path& path) {
        if (!boost::filesystem::exists(path)) {
            result.add_message(
                    "S404",
                    "File not found",
                    fmt::format("The path does not exist: {}", path.string()),
                    true);
            return false;
        }
        std::ifstream file(path.string());
        std::string line;
        auto success = true;
        while (std::getline(file, line)) {
            if (!execute(result, line))
                success = false;
        }
        file.close();
        return success;
    }

    bool environment::save(
            core::result& result,
            const boost::filesystem::path& path) {
        std::ofstream file(path.string());
        for (auto& symbol : _symbol_table.identifiers()) {
            auto value = _symbol_table.get(symbol);
            file << "set " << symbol << " ";
            value->serialize(file);
            file << "\n";
        }
        file << std::endl;
        file.close();
        return true;
    }

    bool environment::on_quit(
            core::result& result,
            const command_handler_context_t& context) {
        result.add_data("command_action", {{"action", std::string("quit")}});
        return true;
    }

    bool environment::on_clear(
            core::result& result,
            const command_handler_context_t& context) {
        result.add_data("command_action", {{"action", std::string("clear")}});
        return true;
    }

    bool environment::on_add_symbol(
            core::result& result,
            const command_handler_context_t& context) {
        auto identifier = boost::get<core::identifier_t>(context.params["name"].front()).value;
        _symbol_table.put(identifier, context.root->children[1]);
        save(result, "global.conf");
        return true;
    }

    bool environment::on_remove_symbol(
            core::result& result,
            const command_handler_context_t& context) {
        auto identifier = boost::get<core::identifier_t>(context.params["name"].front()).value;
        _symbol_table.remove(identifier);
        return true;
    }

    bool environment::on_show_symbol_table(
            core::result& result,
            const command_handler_context_t& context) {
        data_table_t table {};

        table.headers.push_back({"Identifier", 16, 32});
        table.headers.push_back({"Expression", 16, 32});
        table.footers.push_back({"Symbol Count", 32, 64});

        auto identifiers = _symbol_table.identifiers();
        for (const auto& symbol : identifiers) {
            std::stringstream stream;
            _symbol_table.get(symbol)->serialize(stream);
            table.rows.push_back({{symbol, stream.str()}});
        }

        table.rows.push_back({{fmt::format("{} symbols", identifiers.size())}});

        result.add_data("command_result", {{"data", table}});

        return true;
    }

    bool environment::on_assemble(
            core::result& result,
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_evaluate(
            core::result& result,
            const command_handler_context_t& context) {
        const auto& values = context.params["..."];
        for (const auto& param : values) {
            switch (param.which()) {
                case core::variant::types::char_literal: {
                    auto value = boost::get<core::char_literal_t>(param).value;
                    format_numeric_conversion(result, value, context.command.size);
                    break;
                }
                case core::variant::types::numeric_literal: {
                    auto value = boost::get<core::numeric_literal_t>(param).value;
                    format_numeric_conversion(result, value, context.command.size);
                    break;
                }
                case core::variant::types::string_literal: {
                    auto value = boost::get<core::string_literal_t>(param).value;
                    auto dump = ryu::hex_dump(
                            static_cast<const void*>(value.c_str()),
                            value.length());
                    result.add_message("C003", dump);
                    break;
                }
                case core::variant::types::boolean_literal: {
                    auto value = boost::get<core::boolean_literal_t>(param).value;
                    format_numeric_conversion(result, value, core::command_size_flags::byte);
                    break;
                }
                default: {
                    break;
                }
            }
        }
        return true;
    }

    bool environment::on_disassemble(
            core::result& result,
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_dump_memory(
            core::result& result,
            const command_handler_context_t& context) {
        if (core::project::instance() == nullptr) {
            result.add_message(
                    "C033",
                    "no project is loaded; dump memory failed",
                    true);
            return false;
        }

        auto buffer = new uint8_t[128];
        auto addr = boost::get<core::numeric_literal_t>(context.params["addr"].front()).value;

        auto machine = core::project::instance()->machine();
        if (machine == nullptr) {
            result.add_message(
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

        auto dump = ryu::hex_dump(
                static_cast<const void*>(buffer),
                128);

        result.add_message("C003", dump);

        delete[] buffer;

        return true;
    }

    bool environment::on_search_memory(
            core::result& result,
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_fill_memory(
            core::result& result,
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_copy_memory(
            core::result& result,
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_jump_to_address(
            core::result& result,
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_go_to_address(
            core::result& result,
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_list_files(
            core::result& result,
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        auto cwd = current_path();
        if (is_directory(cwd)) {
            auto format_entries = [&](const std::vector<directory_entry*>& list) {
                for (auto entry : list) {
                    std::pair<std::string, std::string> size;
                    if (boost::filesystem::is_regular(entry->path()))
                        size = ryu::size_to_units(boost::filesystem::file_size(entry->path()));
                    result.add_message(
                            "C005",
                            fmt::format("  {0:<41}  {1:>10} {2:<5}",
                                        fmt::format("\"{}\"", entry->path().filename().string()),
                                        size.first,
                                        size.second));
                }
            };
            result.add_message("C005", fmt::format("<rev><bold>  {0:<58} ", cwd.string()));
            result.add_message("C005", fmt::format("<rev><bold>  {0:<41}   {1:<15}", "Filename", "Size"));
            std::vector<directory_entry> entries;
            std::vector<directory_entry*> dir_entries;
            std::vector<directory_entry*> file_entries;
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

        return true;
    }

    bool environment::on_remove_file(
            core::result& result,
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        auto value = boost::get<core::string_literal_t>(context.params["path"].front()).value;
        if (!is_directory(value) && !is_regular_file(value)) {
            result.add_message("C008", fmt::format("invalid path: {}", value), true);
            return false;
        }
        if (remove(value)) {
            result.add_message("C008", fmt::format("{} removed", value));
        } else {
            result.add_message("C008", fmt::format("remove of {} failed", value), true);
        }
        return true;
    }

    bool environment::on_move_file(
            core::result& result,
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        auto src = boost::get<core::string_literal_t>(context.params["src"].front()).value;
        auto dest = boost::get<core::string_literal_t>(context.params["dest"].front()).value;
        // TODO: implement this
        return true;
    }

    bool environment::on_make_directory(
            core::result& result,
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        auto value = boost::get<core::string_literal_t>(context.params["path"].front()).value;
        // TODO: implement this
        return true;
    }

    bool environment::on_change_directory(
            core::result& result,
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        auto value = boost::get<core::string_literal_t>(context.params["path"].front()).value;
        if (!is_directory(value)) {
            result.add_message("C007", fmt::format("invalid path: {}", value), true);
            return false;
        }
        current_path(value);
        auto cwd = current_path();
        result.add_message("C007", fmt::format("working path is now: {}", cwd.string()));
        return true;
    }

    bool environment::on_print_working_directory(
            core::result& result,
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        auto cwd = current_path();
        if (is_directory(cwd)) {
            result.add_message("C006", cwd.string());
        }
        return true;
    }

    bool environment::on_new_project(
            core::result& result,
            const command_handler_context_t& context) {
        return core::project::create(
                result,
                boost::get<core::string_literal_t>(context.params["path"].front()).value);
    }

    bool environment::on_edit_project(
            core::result& result,
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_load_project(
            core::result& result,
            const command_handler_context_t& context) {
        return core::project::load(
                result,
                boost::get<core::string_literal_t>(context.params["path"].front()).value);
    }

    bool environment::on_save_project(
            core::result& result,
            const command_handler_context_t& context) {
        if (core::project::instance() == nullptr) {
            result.add_message(
                    "C033",
                    "no project is loaded; save failed",
                    true);
            return false;
        }
        return core::project::instance()->save(result);
    }

    bool environment::on_close_project(
            core::result& result,
            const command_handler_context_t& context) {
        if (core::project::instance() == nullptr) {
            result.add_message(
                    "C033",
                    "no project is loaded; close failed",
                    true);
            return false;
        }
        return core::project::close(result);
    }

    bool environment::on_clone_project(
            core::result& result,
            const command_handler_context_t& context) {
        if (core::project::instance() == nullptr) {
            result.add_message(
                    "C032",
                    "no project is loaded; clone failed",
                    true);
            return false;
        }
        return core::project::clone(
                result,
                core::project::instance()->path(),
                boost::get<core::string_literal_t>(context.params["path"].front()).value);
    }

    bool environment::on_list_project_files(
            core::result& result,
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_edit_machine(
            core::result& result,
            const command_handler_context_t& context) {
        result.add_data(
                "C023",
                {{"name", boost::get<core::string_literal_t>(context.params["name"].front()).value}});
        return true;
    }

    bool environment::on_list_machines(
            core::result& result,
            const command_handler_context_t& context) {
        auto machines = hardware::registry::instance()->machines();
        result.add_message("C028", "<rev><bold> ID         Name                             Type ");
        for (auto machine : machines) {
            result.add_message(
                    "C028",
                    fmt::format(" {:>5d}      {:<32s} {:<4s}",
                                machine->id(),
                                fmt::format("\"{}\"", machine->name()),
                                "MACH"));
        }
        result.add_message("C028", fmt::format("{} registered machines", machines.size()));
        return true;
    }

    bool environment::on_del_machine(
            core::result& result,
            const command_handler_context_t& context) {
        result.add_data(
                "C027",
                {{"name", boost::get<core::string_literal_t>(context.params["name"].front()).value}});
        return true;
    }

    bool environment::on_use_machine(
            core::result& result,
            const command_handler_context_t& context) {
        if (core::project::instance() == nullptr) {
            result.add_message(
                    "C034",
                    "no project is loaded; use machine failed",
                    true);
            return false;
        }
        auto machine_name = boost::get<core::string_literal_t>(context.params["name"].front()).value;
        auto machine = hardware::registry::instance()->find_machine(machine_name);
        if (machine == nullptr) {
            result.add_message(
                    "C034",
                    "no machine exists with that name",
                    true);
            return false;
        }
        core::project::instance()->machine(machine);
        return !result.is_failed();
    }

    bool environment::on_open_editor(
            core::result& result,
            const command_handler_context_t& context) {
        core::parameter_dict dict;

        auto name = boost::get<core::string_literal_t>(context.params["name"].front()).value;
        auto type = boost::get<core::identifier_t>(context.params["type"].front()).value;

        std::string action;
        if (type == "MACH") {
            action = "edit_machine";
        } else if (type == "TEXT") {
            action = "edit_source";
        } else if (type == "DATA") {
            action = "edit_memory";
        }

        result.add_data("command_action", {
            {"action", action},
            {"name", name}
        });

        return true;
    }

    bool environment::on_source_editor(
            core::result& result,
            const command_handler_context_t& context) {
        result.add_data("command_action", {{"action", std::string("edit_source")}});
        return true;
    }

    bool environment::on_memory_editor(
            core::result& result,
            const command_handler_context_t& context) {
        result.add_data("command_action", {{"action", std::string("edit_memory")}});
        return true;
    }

    bool environment::on_sprite_editor(
            core::result& result,
            const command_handler_context_t& context) {
        result.add_data("command_action", {{"action", std::string("edit_sprites")}});
        return true;
    }

    bool environment::on_tile_editor(
            core::result& result,
            const command_handler_context_t& context) {
        result.add_data("command_action", {{"action", std::string("edit_tiles")}});
        return true;
    }

    bool environment::on_background_editor(
            core::result& result,
            const command_handler_context_t& context) {
        result.add_data("command_action", {{"action", std::string("edit_backgrounds")}});
        return true;
    }

    bool environment::on_tracker(
            core::result& result,
            const command_handler_context_t& context) {
        result.add_data("command_action", {{"action", std::string("edit_music")}});
        return true;
    }

    bool environment::on_sounds(
            core::result& result,
            const command_handler_context_t& context) {
        result.add_data("command_action", {{"action", std::string("edit_sounds")}});
        return true;
    }

    bool environment::on_read_binary_to_memory(
            core::result& result,
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_write_memory_to_binary(
            core::result& result,
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_read_text(
            core::result& result,
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        auto value = boost::get<core::string_literal_t>(context.params["path"].front()).value;
        if (!is_regular_file(value)) {
            result.add_message("C021", fmt::format("invalid path: {}", value), true);
            return false;
        }
        result.add_message("C021", value);
        return true;
    }

    bool environment::on_write_text(
            core::result& result,
            const command_handler_context_t& context) {
        using namespace boost::filesystem;

        std::string value {"(default)"};
        if (!context.root->children.empty()) {
            value = boost::get<core::string_literal_t>(context.params["path"].front()).value;
            if (!is_regular_file(value)) {
                result.add_message("C022", fmt::format("invalid path: {}", value), true);
                return false;
            }
        }
        result.add_message("C022", value);
        return true;
    }

    bool environment::on_goto_line(
            core::result& result,
            const command_handler_context_t& context) {
        result.add_message(
                "C020",
                std::to_string(boost::get<core::numeric_literal_t>(context.params["line"].front()).value));
        return true;
    }

    bool environment::on_find_text(
            core::result& result,
            const command_handler_context_t& context) {
        return true;
    }

    bool environment::on_register_editor(
            core::result& result,
            const command_handler_context_t& context) {
        result.add_data("command_action", {{"action", std::string("edit_cpu")}});
        return true;
    }

    bool environment::on_help(
            core::result& result,
            const command_handler_context_t& context) {
        constexpr uint8_t command_column_width = 20;
        constexpr uint8_t help_column_width = 50;

        const auto& commands = core::command_parser::command_catalog();
        result.add_message("C030", "<rev><bold> Command                          Help                                   ");

        for (const auto& c : commands) {
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

            auto formatted_text = core::text_formatter::format_text_left_padded(
                    stream.str(),
                    command_column_width);

            result.add_message("C030",
                               fmt::format(" {} {}\n",
                                           core::text_formatter::formatted_text_to_string(formatted_text),
                                           word_wrap(c.second.help, help_column_width, command_column_width + 2)));
        }

        result.add_message("C030", fmt::format("{} available commands", commands.size()));
        return true;
    }

}