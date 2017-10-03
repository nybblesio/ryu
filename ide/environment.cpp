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

namespace ryu::ide {

    static void format_numeric_conversion(
            core::result& result,
            int32_t value,
            core::command_t::sizes size) {
        std::string ascii;
        std::string fmt_spec;

        uint32_t mask = 0;
        auto byte_count = 0;
        switch (size) {
            case core::command_t::byte:
                byte_count = 1;
                mask = 0xff;
                fmt_spec = "${0:02x} {1:>3} \"{2:>1}\" %{0:08b}";
                break;
            case core::command_t::word:
                byte_count = 2;
                mask = 0xffff;
                fmt_spec = "${0:04x} {1:>5} \"{2:>2}\" %{0:016b}";
                break;
            case core::command_t::dword:
                byte_count = 4;
                mask = 0xffffffff;
                fmt_spec = "${0:08x} {1:>10} \"{2:>4}\" %{0:032b}";
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

    environment* environment::instance() {
        static environment instance;
        return &instance;
    }

    bool environment::execute(
            core::result& result,
            const std::string& line) {
        using namespace boost::filesystem;

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

        std::map<std::string, std::vector<core::variant_t>> params;
        if (root->children.empty()) {
            for (const auto& param_spec : command.spec.params) {
                if (param_spec.required) {
                    result.add_message(
                            "C801",
                            fmt::format("the parameter {{italic}}'{}'{{}} is required.", param_spec.name),
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
                                fmt::format("the parameter {{italic}}'{}'{{}} is required.", param_spec.name),
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
                                fmt::format("the parameter {{italic}}'{}'{{}} must be of type {{italic}}'{}'{{}}.",
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

        switch (command.spec.type) {
            case core::command_types::quit: {
                result.add_message("C001", "Goodbye!");
                break;
            }
            case core::command_types::clear: {
                result.add_message("C004", "Clear screen buffer");
                break;
            }
            case core::command_types::evaluate: {
                const auto& values = params["..."];
                for (const auto& param : values) {
                    switch (param.which()) {
                        case core::variant::types::char_literal: {
                            auto value = boost::get<core::char_literal_t>(param).value;
                            format_numeric_conversion(result, value, command.size);
                            break;
                        }
                        case core::variant::types::numeric_literal: {
                            auto value = boost::get<core::numeric_literal_t>(param).value;
                            format_numeric_conversion(result, value, command.size);
                            break;
                        }
                        case core::variant::types::string_literal: {
                            auto value = boost::get<core::string_literal_t>(param).value;
                            auto dump = hex_dump(
                                    static_cast<const void*>(value.c_str()),
                                    value.length());
                            result.add_message("C003", dump);
                            break;
                        }
                        case core::variant::types::boolean_literal: {
                            auto value = boost::get<core::boolean_literal_t>(param).value;
                            format_numeric_conversion(result, value, core::command_t::sizes::byte);
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                }
                break;
            }
            case core::command_types::change_directory: {
                auto value = boost::get<core::string_literal_t>(params["path"].front()).value;
                if (!is_directory(value)) {
                    result.add_message("C007", fmt::format("invalid path: {}", value), true);
                    return false;
                }
                current_path(value);
                auto cwd = current_path();
                result.add_message("C007", fmt::format("working path is now: {}", cwd.string()));
                break;
            }
            case core::command_types::dir: {
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
            case core::command_types::remove_file: {
                auto value = boost::get<core::string_literal_t>(params["path"].front()).value;
                if (!is_directory(value) && !is_regular_file(value)) {
                    result.add_message("C008", fmt::format("invalid path: {}", value), true);
                    return false;
                }
                if (remove(value)) {
                    result.add_message("C008", fmt::format("{} removed", value));
                } else {
                    result.add_message("C008", fmt::format("remove of {} failed", value), true);
                }
                break;
            }
            case core::command_types::print_working_directory: {
                auto cwd = current_path();
                if (is_directory(cwd)) {
                    result.add_message("C006", cwd.string());
                }
                break;
            }
            case core::command_types::read_text: {
                auto value = boost::get<core::string_literal_t>(params["path"].front()).value;
                if (!is_regular_file(value)) {
                    result.add_message("C021", fmt::format("invalid path: {}", value), true);
                    return false;
                }
                result.add_message("C021", value);
                break;
            }
            case core::command_types::write_text: {
                std::string value {"(default)"};
                if (!root->children.empty()) {
                    value = boost::get<core::string_literal_t>(params["path"].front()).value;
                    if (!is_regular_file(value)) {
                        result.add_message("C022", fmt::format("invalid path: {}", value), true);
                        return false;
                    }
                }
                result.add_message("C022", value);
                break;
            }
            case core::command_types::memory_editor: {
                result.add_data("C024", {});
                break;
            }
            case core::command_types::text_editor: {
                result.add_data("C002", {});
                break;
            }
            case core::command_types::open_editor: {
                core::parameter_dict dict;
                dict["name"] = boost::get<core::string_literal_t>(params["name"].front()).value;
                dict["type"] = boost::get<core::identifier_t>(params["type"].front()).value;
                result.add_data("C030", dict);
                break;
            }
            case core::command_types::machines_list: {
                auto machines = hardware::registry::instance()->machines();
                result.add_message("C028", "{rev}{bold}  ID Name                             Type ");
                for (auto machine : machines) {
                    result.add_message(
                            "C028",
                            fmt::format(" {:>3d} {:<32s} {:<4s}",
                                        machine->id(),
                                        fmt::format("\"{}\"", machine->name()),
                                        "MACH"));
                }
                result.add_message("C028", fmt::format("{} registered machines", machines.size()));
                break;
            }
            case core::command_types::machine_editor: {
                result.add_data(
                        "C023",
                        {{"name", boost::get<core::string_literal_t>(params["name"].front()).value}});
                break;
            }
            case core::command_types::use_machine: {
                result.add_data(
                        "C026",
                        {{"name", boost::get<core::string_literal_t>(params["name"].front()).value}});
                break;
            }
            case core::command_types::del_machine: {
                result.add_data(
                        "C027",
                        {{"name", boost::get<core::string_literal_t>(params["name"].front()).value}});
                break;
            }
            case core::command_types::goto_line: {
                result.add_message(
                        "C020",
                        std::to_string(boost::get<core::numeric_literal_t>(params["line"].front()).value));
                break;
            }
            case core::command_types::add_symbol: {
                auto identifier = boost::get<core::identifier_t>(params["name"].front()).value;
                _symbol_table.put(identifier, root->children[1]);
                save(result, "global.conf");
                break;
            }
            case core::command_types::remove_symbol: {
                auto identifier = boost::get<core::identifier_t>(params["name"].front()).value;
                _symbol_table.remove(identifier);
                break;
            }
            case core::command_types::show_symbol_table: {
                result.add_message("C029", "{rev}{bold} Identifier                       Value                            ");
                auto identifiers = _symbol_table.identifiers();
                for (const auto& symbol : identifiers) {
                    std::stringstream stream;
                    _symbol_table.get(symbol)->serialize(stream);
                    result.add_message(
                            "C029",
                            fmt::format(" {:<32s} {:<32s}", symbol, stream.str()));
                }
                result.add_message("C029", fmt::format("{} symbols", identifiers.size()));
                break;
            }
            default: {
                result.add_message("C400", "Command not implemented.", true);
                return false;
            }
        }

        return true;
    }

    core::symbol_table* environment::symbol_table() {
        return &_symbol_table;
    }

    bool environment::load(core::result& result, const boost::filesystem::path& path) {
        if (!boost::filesystem::exists(path)) {
            result.add_message(
                    "S404",
                    "File not found",
                    fmt::format("The path does not exist: {}", path.string()), true);
            result.fail();
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

    bool environment::save(core::result& result, const boost::filesystem::path& path) {
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

}
