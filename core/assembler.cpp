//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <iostream>
#include <hardware/component.h>
#include "project.h"
#include "assembler.h"
#include "symbol_table.h"

namespace ryu::core {

    assembler::assembler() : _evaluator(this) {
        _parser.register_operators();
    }

    bool assembler::assemble(
            core::result& result,
            const parser_input_t& input) {
        core::project::instance()
                ->machine()
                ->set_write_latches(true);

        _listing.reset();
        assemble_stream(result, input);
        _listing.finalize();

        core::project::instance()
                ->machine()
                ->set_write_latches(false);

        return !result.is_failed();
    }

    bool assembler::load_target(
            core::result& result,
            const std::string& name) {
        auto project = core::project::instance();
        if (project == nullptr) {
            result.add_message(
                    "C033",
                    "no project is loaded; unable to load component",
                    true);
            return false;
        }

        auto machine = project->machine();
        if (machine == nullptr) {
            result.add_message(
                    "C033",
                    "project has no machine associated",
                    true);
            return false;
        }

        auto component = machine->find_component(name);
        if (component == nullptr) {
            result.add_message(
                    "C033",
                    "could not find component on machine",
                    true);
            return false;
        }

        _target = component;

        return true;
    }

    bool assembler::assemble_stream(
            core::result& result,
            const parser_input_t& input) {
        auto program_node = _parser.parse(input);
        auto parser_result = _parser.result();
        if (parser_result.is_failed()) {
            for (auto& msg : parser_result.messages())
                result.add_message(
                        msg.code(),
                        msg.message(),
                        msg.details(),
                        msg.is_error());
        } else {
            if (program_node != nullptr) {
                _listing.begin_assembly_scope(input);

                if (!_evaluator.pass1_transform(result, program_node)) {

                }

                _listing.end_assembly_scope();
            }
        }

        return !result.is_failed();
    }

    void assembler::align(uint8_t size) {
        auto offset = _location_counter % size;
        _location_counter = offset ?
                            _location_counter + (size - offset) :
                            _location_counter;
    }

    std::vector<uint8_t> assembler::write_data(
            directive_t::data_sizes size,
            uint32_t value) {
        auto machine = core::project::instance()->machine();
        switch (size) {
            case directive_t::data_sizes::word: {
                auto data_list = machine->mapper()->write_word(
                        _location_counter,
                        static_cast<uint16_t>(value),
                        _target->ic()->endianess());
                _location_counter += 2;
                return data_list;
            }
            case directive_t::data_sizes::dword: {
                auto data_list = machine->mapper()->write_dword(
                        _location_counter,
                        value,
                        _target->ic()->endianess());
                _location_counter += 4;
                return data_list;
            }
            default: {
                auto data_byte = static_cast<uint8_t>(value);
                machine->mapper()->write_byte(
                        _location_counter++,
                        data_byte);
                return {data_byte};
            }
        }
    }

    hardware::component* assembler::target() {
        return _target;
    }

    uint32_t assembler::location_counter() const {
        return _location_counter;
    }

    core::assembly_listing& assembler::listing() {
        return _listing;
    }

    core::symbol_table* assembler::symbol_table() {
        return _symbol_table;
    }

    void assembler::location_counter(uint32_t value) {
        _location_counter = value;
    }

    void assembler::symbol_table(core::symbol_table* value) {
        _symbol_table = value;
        _parser.symbol_table(_symbol_table);
        _evaluator.symbol_table(_symbol_table);
    }

    std::vector<uint8_t> assembler::write_data(const std::string& value) {
        std::vector<uint8_t> data {};

        auto machine = core::project::instance()->machine();
        for (const auto& c : value) {
            auto data_byte = static_cast<uint8_t>(c);
            machine
                    ->mapper()
                    ->write_byte(_location_counter++, data_byte);
            data.push_back(data_byte);
        }

        return data;
    }

    void assembler::increment_location_counter(directive_t::data_sizes size) {
        switch (size) {
            case directive_t::byte:
            case directive_t::ascii:
                ++_location_counter;
                break;
            case directive_t::word:
                _location_counter += 2;
                break;
            case directive_t::dword:
                _location_counter += 4;
                break;
            default:
                break;
        }
    }

}