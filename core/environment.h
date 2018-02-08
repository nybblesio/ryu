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

#include <core/result.h>
#include "project.h"

namespace ryu::core {

    class assembler;
    class evaluator;
    class symbol_table;
    class command_parser;

    class environment {
    public:
        environment();

        virtual ~environment();

        std::string name() const;

        bool assemble(
                core::result& result,
                core::project_file* file);

        bool apply_environment(
                core::result& result,
                core::project_file* file);

        core::symbol_table* symbol_table();

        bool assemble(core::result& result);

        void name(const std::string& value);

        bool execute(core::result& result, const std::string& line);

    private:
        void format_data_bytes(
                std::stringstream& stream,
                uint32_t address,
                const byte_list& bytes);

        data_table_t create_symbol_table();

        bool has_valid_project(core::result& result);

        bool has_valid_project_and_machine(core::result& result);

        bool has_valid_project_machine_and_target(core::result& result);

    private:
        // ----------------------------------------------------------
        // system commands
        // ----------------------------------------------------------
        bool on_quit(
                const command_handler_context_t& context);

        bool on_help(
                const command_handler_context_t& context);

        bool on_clear(
                const command_handler_context_t& context);

        bool on_evaluate(
                const command_handler_context_t& context);

        // ----------------------------------------------------------
        // environment commands
        // ----------------------------------------------------------
        bool on_assemble(
                const command_handler_context_t& context);

        bool on_set_target(
                const command_handler_context_t& context);

        bool on_add_symbol(
                const command_handler_context_t& context);

        bool on_edit_environment(
                const command_handler_context_t& context);

        bool on_disassemble(
                const command_handler_context_t& context);

        bool on_remove_symbol(
                const command_handler_context_t& context);

        bool on_go_to_address(
                const command_handler_context_t& context);

        bool on_apply_environment(
                const command_handler_context_t& context);

        bool on_new_environment(
                const command_handler_context_t& context);

        bool on_jump_to_address(
                const command_handler_context_t& context);

        bool on_show_symbol_table(
                const command_handler_context_t& context);

        bool on_list_environments(
                const command_handler_context_t& context);

        bool on_remove_environment(
                const command_handler_context_t& context);

        bool on_switch_environment(
                const command_handler_context_t& context);

        // ----------------------------------------------------------
        // memory commands
        // ----------------------------------------------------------
        bool on_peek_memory(
                const command_handler_context_t& context);

        bool on_poke_memory(
                const command_handler_context_t& context);

        bool on_fill_memory(
                const command_handler_context_t& context);

        bool on_copy_memory(
                const command_handler_context_t& context);

        bool on_dump_memory(
                const command_handler_context_t& context);

        bool on_search_memory(
                const command_handler_context_t& context);

        bool on_memory_editor(
                const command_handler_context_t& context);

        bool on_read_binary_to_memory(
                const command_handler_context_t& context);

        bool on_write_memory_to_binary(
                const command_handler_context_t& context);

        // ----------------------------------------------------------
        // file system commands
        // ----------------------------------------------------------
        bool on_move_file(
                const command_handler_context_t& context);

        bool on_list_files(
                const command_handler_context_t& context);

        bool on_remove_file(
                const command_handler_context_t& context);

        bool on_make_directory(
                const command_handler_context_t& context);

        bool on_change_directory(
                const command_handler_context_t& context);

        bool on_print_working_directory(
                const command_handler_context_t& context);

        // ----------------------------------------------------------
        // machine commands
        // ----------------------------------------------------------
        bool on_use_machine(
                const command_handler_context_t& context);

        bool on_edit_machine(
                const command_handler_context_t& context);

        bool on_list_machines(
                const command_handler_context_t& context);

        bool on_delete_machine(
                const command_handler_context_t& context);

        bool on_memory_map(
                const command_handler_context_t& context);

        // ----------------------------------------------------------
        // project & project file commands
        // ----------------------------------------------------------
        bool on_new_project(
                const command_handler_context_t& context);

        bool on_edit_project(
                const command_handler_context_t& context);

        bool on_load_project(
                const command_handler_context_t& context);

        bool on_save_project(
                const command_handler_context_t& context);

        bool on_close_project(
                const command_handler_context_t& context);

        bool on_clone_project(
                const command_handler_context_t& context);

        bool on_new_project_file(
                const command_handler_context_t& context);

        bool on_save_project_file(
                const command_handler_context_t& context);

        bool on_list_project_files(
                const command_handler_context_t& context);

        bool on_remove_project_file(
                const command_handler_context_t& context);

        // ----------------------------------------------------------
        // editor commands
        // ----------------------------------------------------------
        bool on_find_text(
                const command_handler_context_t& context);

        bool on_read_text(
                const command_handler_context_t& context);

        bool on_goto_line(
                const command_handler_context_t& context);

        bool on_write_text(
                const command_handler_context_t& context);

        bool on_tile_editor(
                const command_handler_context_t& context);

        bool on_open_editor(
                const command_handler_context_t& context);

        bool on_sample_editor(
                const command_handler_context_t& context);

        bool on_module_editor(
                const command_handler_context_t& context);

        bool on_sprite_editor(
                const command_handler_context_t& context);

        bool on_source_editor(
                const command_handler_context_t& context);

        bool on_register_editor(
                const command_handler_context_t& context);

        bool on_background_editor(
                const command_handler_context_t& context);

    private:
        static command_handler_dict _command_handlers;

        std::string _name;
        std::unique_ptr<core::assembler> _assembler;
        std::unique_ptr<core::evaluator> _evaluator;
        std::unique_ptr<core::symbol_table> _symbol_table;
        std::unique_ptr<core::command_parser> _command_parser;
    };

};

