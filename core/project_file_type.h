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

#include <string>

namespace ryu::core {

    class project_file_type {
    public:
        enum codes {
            uninitialized,
            source,
            data,
            tiles,
            sprites,
            module,
            sample,
            background,
            environment,
        };

        static std::string code_to_action(const std::string& code);

        static bool does_type_require_project(const std::string& code);

        static std::string type_to_code(project_file_type::codes code);

        static project_file_type::codes code_to_type(const std::string& code);
    };

};

