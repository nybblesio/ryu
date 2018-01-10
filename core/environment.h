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
        using command_handler_callable = std::function<bool (
                core::result&,
                core::command_t&,
                core::command_parameter_dict&,
                const core::ast_node_shared_ptr&)>;

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
        bool on_quit(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_clear(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_add_symbol(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_remove_symbol(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_show_symbol_table(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_assemble(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_evaluate(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_disassemble(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_hex_dump(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_search_memory(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_fill_memory(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_copy_memory(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_jump_to_address(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_go_to_address(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_dir(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_remove_file(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_change_directory(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_print_working_directory(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_new_project(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_load_project(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_save_project(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_clone_project(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_find_text(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_goto_line(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_write_text(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_read_text(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_write_memory_to_binary(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_read_binary_to_memory(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_sounds(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_tracker(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_background_editor(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_tile_editor(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_sprite_editor(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_memory_editor(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_source_editor(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_open_editor(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_use_machine(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_del_machine(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_machines_list(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_machine_editor(result& result, command_t& command, command_parameter_dict& params, const core::ast_node_shared_ptr& root);

        bool on_register_editor(result& result, command_t& command, command_parameter_dict& params, const ast_node_shared_ptr& root);

    private:
        core::symbol_table _symbol_table;
    };

};

