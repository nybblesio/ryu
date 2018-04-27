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
#include "project_file_type.h"

namespace ryu::core {

    namespace fs = boost::filesystem;

    class project_file {
    public:
        static project_file_shared_ptr load(
            core::result& result,
            YAML::Node& node);

        project_file() = default;

        project_file(
                uint32_t id,
                const fs::path& path,
                project_file_type::codes type);

        bool read(
                core::result& result,
                std::iostream& stream);

        bool write(
                core::result& result,
                std::iostream& stream);

        bool create_stub_file(
                core::result& result,
                const fs::path& path);

        bool dirty() const;

        uint32_t id() const;

        fs::path path() const;

        void dirty(bool value);

        std::string name() const;

        uint16_t sequence() const;

        fs::path full_path() const;

        bool should_assemble() const;

        void sequence(uint16_t value);

        void should_assemble(bool flag);

        void path(const fs::path& value);

        project_file_type::codes type() const;

        void type(project_file_type::codes value);

        bool save(core::result& result, YAML::Emitter& emitter);

    private:
        uint32_t _id {};
        fs::path _path {};
        bool _dirty = false;
        uint16_t _sequence = 0;
        bool _should_assemble = false;
        project_file_type::codes _type = project_file_type::codes::uninitialized;
    };

};

