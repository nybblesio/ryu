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

    struct command_handler_context_t {
        core::command_t& command;
        core::command_parameter_dict& params;
        const core::ast_node_shared_ptr& root;
    };

    class environment {
    public:
        using command_handler_callable = std::function<bool (
                core::result&,
                const command_handler_context_t&)>;

        environment() = default;

        ~environment() = default;

        core::symbol_table* symbol_table();

        bool assemble(core::result& result, core::project& project);

        bool execute(core::result& result, const std::string& line);

        // N.B. loads a .s file that has environment config
        bool load(core::result& result, const boost::filesystem::path& path);

        // N.B. saves a .s file that has environment config
        bool save(core::result& result, const boost::filesystem::path& path);

    private:
        bool on_quit(result& result, const command_handler_context_t& context);

        bool on_clear(result& result, const command_handler_context_t& context);

        bool on_add_symbol(result& result, const command_handler_context_t& context);

        bool on_remove_symbol(result& result, const command_handler_context_t& context);

        bool on_show_symbol_table(result& result, const command_handler_context_t& context);

        bool on_assemble(result& result, const command_handler_context_t& context);

        bool on_evaluate(result& result, const command_handler_context_t& context);

        bool on_disassemble(result& result, const command_handler_context_t& context);

        bool on_hex_dump(result& result, const command_handler_context_t& context);

        bool on_search_memory(result& result, const command_handler_context_t& context);

        bool on_fill_memory(result& result, const command_handler_context_t& context);

        bool on_copy_memory(result& result, const command_handler_context_t& context);

        bool on_jump_to_address(result& result, const command_handler_context_t& context);

        bool on_go_to_address(result& result, const command_handler_context_t& context);

        bool on_move_file(result& result, const command_handler_context_t& context);

        bool on_list_files(result& result, const command_handler_context_t& context);

        bool on_remove_file(result& result, const command_handler_context_t& context);

        bool on_change_directory(result& result, const command_handler_context_t& context);

        bool on_make_directory(result& result, const command_handler_context_t& context);

        bool on_print_working_directory(result& result, const command_handler_context_t& context);

        bool on_new_project(result& result, const command_handler_context_t& context);

        bool on_load_project(result& result, const command_handler_context_t& context);

        bool on_save_project(result& result, const command_handler_context_t& context);

        bool on_close_project(result& result, const command_handler_context_t& context);

        bool on_clone_project(result& result, const command_handler_context_t& context);

        bool on_find_text(result& result, const command_handler_context_t& context);

        bool on_goto_line(result& result, const command_handler_context_t& context);

        bool on_write_text(result& result, const command_handler_context_t& context);

        bool on_read_text(result& result, const command_handler_context_t& context);

        bool on_write_memory_to_binary(result& result, const command_handler_context_t& context);

        bool on_read_binary_to_memory(result& result, const command_handler_context_t& context);

        bool on_sounds(result& result, const command_handler_context_t& context);

        bool on_tracker(result& result, const command_handler_context_t& context);

        bool on_background_editor(result& result, const command_handler_context_t& context);

        bool on_tile_editor(result& result, const command_handler_context_t& context);

        bool on_sprite_editor(result& result, const command_handler_context_t& context);

        bool on_memory_editor(result& result, const command_handler_context_t& context);

        bool on_source_editor(result& result, const command_handler_context_t& context);

        bool on_open_editor(result& result, const command_handler_context_t& context);

        bool on_use_machine(result& result, const command_handler_context_t& context);

        bool on_del_machine(result& result, const command_handler_context_t& context);

        bool on_list_machines(result& result, const command_handler_context_t& context);

        bool on_edit_machine(result& result, const command_handler_context_t& context);

        bool on_register_editor(result& result, const command_handler_context_t& context);

        bool on_help(result& result, const command_handler_context_t& context);

        bool on_list_project_files(result& result, const command_handler_context_t& context);

        bool on_edit_project(result& result, const command_handler_context_t& context);

    private:
        core::symbol_table _symbol_table;
    };

};

