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
#include <core/symbol_table.h>
#include "project.h"

namespace ryu::core {

    class environment {
    public:
        explicit environment(const std::string& name);

        ~environment() = default;

        std::string name() const;

        void name(const std::string& value);

        bool load(core::result& result);

        bool save(core::result& result);

        core::symbol_table* symbol_table();

        bool assemble(core::result& result);

        bool execute(core::result& result, const std::string& line);

    private:
        bool on_quit(const command_handler_context_t& context);

        bool on_clear(const command_handler_context_t& context);

        bool on_add_symbol(const command_handler_context_t& context);

        bool on_remove_symbol(const command_handler_context_t& context);

        bool on_show_symbol_table(const command_handler_context_t& context);

        bool on_assemble(const command_handler_context_t& context);

        bool on_evaluate(const command_handler_context_t& context);

        bool on_disassemble(const command_handler_context_t& context);

        bool on_dump_memory(const command_handler_context_t& context);

        bool on_search_memory(const command_handler_context_t& context);

        bool on_fill_memory(const command_handler_context_t& context);

        bool on_copy_memory(const command_handler_context_t& context);

        bool on_jump_to_address(const command_handler_context_t& context);

        bool on_go_to_address(const command_handler_context_t& context);

        bool on_move_file(const command_handler_context_t& context);

        bool on_list_files(const command_handler_context_t& context);

        bool on_remove_file(const command_handler_context_t& context);

        bool on_change_directory(const command_handler_context_t& context);

        bool on_make_directory(const command_handler_context_t& context);

        bool on_print_working_directory(const command_handler_context_t& context);

        bool on_new_project(const command_handler_context_t& context);

        bool on_load_project(const command_handler_context_t& context);

        bool on_save_project(const command_handler_context_t& context);

        bool on_close_project(const command_handler_context_t& context);

        bool on_clone_project(const command_handler_context_t& context);

        bool on_find_text(const command_handler_context_t& context);

        bool on_goto_line(const command_handler_context_t& context);

        bool on_write_text(const command_handler_context_t& context);

        bool on_read_text(const command_handler_context_t& context);

        bool on_write_memory_to_binary(const command_handler_context_t& context);

        bool on_read_binary_to_memory(const command_handler_context_t& context);

        bool on_sample_editor(const command_handler_context_t& context);

        bool on_module_editor(const command_handler_context_t& context);

        bool on_background_editor(const command_handler_context_t& context);

        bool on_tile_editor(const command_handler_context_t& context);

        bool on_sprite_editor(const command_handler_context_t& context);

        bool on_memory_editor(const command_handler_context_t& context);

        bool on_source_editor(const command_handler_context_t& context);

        bool on_open_editor(const command_handler_context_t& context);

        bool on_use_machine(const command_handler_context_t& context);

        bool on_delete_machine(const command_handler_context_t& context);

        bool on_list_machines(const command_handler_context_t& context);

        bool on_edit_machine(const command_handler_context_t& context);

        bool on_register_editor(const command_handler_context_t& context);

        bool on_help(const command_handler_context_t& context);

        bool on_new_project_file(const command_handler_context_t& context);

        bool on_remove_project_file(const command_handler_context_t& context);

        bool on_list_project_files(const command_handler_context_t& context);

        bool on_edit_project(const command_handler_context_t& context);

    private:
        static command_handler_dict _command_handlers;

        std::string _name;
        core::symbol_table _symbol_table;
    };

};

