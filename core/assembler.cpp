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

    bool assembler::assemble(
            core::result& result,
            std::string& input) {
        core::project::instance()
                ->machine()
                ->set_write_latches(true);

        _listing.begin_assembly(input);

        auto program_node = _parser.parse(input);
        auto parser_result = _parser.result();
        if (program_node == nullptr && parser_result.is_failed()) {
            for (auto& msg : parser_result.messages())
                result.add_message(
                        msg.code(),
                        msg.message(),
                        msg.details(),
                        msg.is_error());
            return false;
        }

        if (program_node != nullptr) {
            if (!_evaluator.pass1_transform(result, this, program_node)) {

            }
        }

        core::project::instance()
                ->machine()
                ->set_write_latches(false);

        _listing.end_assembly();

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

    void assembler::write_data(
            directive_t::data_sizes size,
            uint32_t value) {
        auto machine = core::project::instance()->machine();
        switch (size) {
            case directive_t::data_sizes::word:
                machine->mapper()->write_word(
                        _location_counter,
                        static_cast<uint16_t>(value),
                        _target->ic()->endianess());
                _location_counter += 2;
                break;
            case directive_t::data_sizes::dword:
                machine->mapper()->write_dword(
                        _location_counter,
                        value,
                        _target->ic()->endianess());
                _location_counter += 4;
                break;
            default:
                machine->mapper()->write_byte(
                        _location_counter++,
                        static_cast<uint8_t>(value));
                break;
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

    void assembler::write_data(const std::string& value) {
        auto machine = core::project::instance()->machine();
        for (const auto& c : value)
            machine->mapper()->write_byte(_location_counter++, (uint8_t) c);
    }

    void assembler::symbol_table(core::symbol_table* value) {
        _symbol_table = value;
        _parser.symbol_table(_symbol_table);
        _evaluator.symbol_table(_symbol_table);
    }

}