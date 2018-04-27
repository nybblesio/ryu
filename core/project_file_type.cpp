//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <map>
#include <boost/algorithm/string.hpp>
#include "project_file_type.h"

namespace ryu::core {

    static std::map<uint8_t, std::string> s_type_to_code = {
            {project_file_type::source,      "text"},
            {project_file_type::data,        "data"},
            {project_file_type::tiles,       "tiles"},
            {project_file_type::sprites,     "sprites"},
            {project_file_type::module,      "module"},
            {project_file_type::sample,      "sample"},
            {project_file_type::environment, "environ"},
            {project_file_type::background,  "bg"},
            {project_file_type::actor,       "actor"},
            {project_file_type::palette,     "palette"}
    };

    static std::map<std::string, uint8_t> s_code_to_type = {
            {"text",    project_file_type::source},
            {"data",    project_file_type::data},
            {"tiles",   project_file_type::tiles},
            {"sprites", project_file_type::sprites},
            {"module",  project_file_type::module},
            {"sample",  project_file_type::sample},
            {"bg",      project_file_type::background},
            {"environ", project_file_type::environment},
            {"actor",   project_file_type::actor},
            {"palette", project_file_type::palette}
    };

    static std::map<std::string, core::system_commands::types> s_code_to_action = {
            {"mach",    core::system_commands::edit_machine},
            {"text",    core::system_commands::edit_source},
            {"environ", core::system_commands::edit_source},
            {"data",    core::system_commands::edit_memory},
            {"tiles",   core::system_commands::edit_tiles},
            {"sprites", core::system_commands::edit_sprites},
            {"bg",      core::system_commands::edit_backgrounds},
            {"module",  core::system_commands::edit_music},
            {"sample",  core::system_commands::edit_sounds},
            {"palette", core::system_commands::edit_palette},
            {"actor",   core::system_commands::edit_actor}
    };

    core::system_commands::types project_file_type::code_to_action(const std::string& code) {
        auto it = s_code_to_action.find(boost::to_lower_copy<std::string>(code));
        if (it == s_code_to_action.end())
            return core::system_commands::unknown;
        return it->second;
    }

    std::string project_file_type::type_to_code(project_file_type::codes code) {
        auto it = s_type_to_code.find(code);
        if (it == s_type_to_code.end())
            return "UNKNOWN";
        return it->second;
    }

    bool project_file_type::does_type_require_project(const std::string& code) {
        return !(boost::to_lower_copy<std::string>(code) == "mach");
    }

    project_file_type::codes project_file_type::code_to_type(const std::string& code) {
        auto it = s_code_to_type.find(boost::to_lower_copy<std::string>(code));
        if (it == s_code_to_type.end())
            return project_file_type::uninitialized;
        return static_cast<project_file_type::codes>(it->second);
    }

}