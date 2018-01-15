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
#include <yaml-cpp/yaml.h>
#include <hardware/component.h>
#include <boost/filesystem.hpp>
#include "result.h"
#include "core_types.h"

namespace ryu::core {

    namespace fs = boost::filesystem;

    class project_file {
    public:
        static project_file load(
                core::result& result,
                const hardware::machine* machine,
                YAML::Node& node);

        project_file() = default;

        project_file(
                const fs::path& path,
                core::project_file_types type);

        bool dirty() const;

        fs::path path() const;

        void dirty(bool value);

        hardware::component* cpu();

        void path(const fs::path& value);

        void cpu(hardware::component* value);

        core::project_file_types type() const;

        void type(core::project_file_types value);

        bool save(core::result& result, YAML::Emitter& emitter);

    private:
        fs::path _path {};
        bool _dirty = false;
        hardware::component* _cpu = nullptr;
        core::project_file_types _type = core::project_file_types::assembly_source;
    };

};

