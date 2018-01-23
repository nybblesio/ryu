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
#include <utility>
#include <hardware/machine.h>
#include <boost/filesystem/path.hpp>
#include "result.h"

namespace ryu::core {

    namespace fs = boost::filesystem;

    class project {
    public:
        using project_changed_callable = std::function<void ()>;

        static bool load(
                core::result& result,
                const fs::path& path);

        static bool create(
                core::result& result,
                const fs::path& path);

        static bool clone(
                core::result& result,
                const fs::path& source_path,
                const fs::path& target_path);

        static core::project* instance();

        static bool close(core::result& result);

        static void add_listener(const project_changed_callable& callable);

        bool dirty() const;

        fs::path path() const;

        void remove_all_files();

        std::string name() const;

        hardware::machine* machine();

        void remove_file(uint32_t id);

        std::string description() const;

        bool save(core::result& result);

        project_file* active_environment();

        void name(const std::string& value);

        project_file* find_file(uint32_t id);

        const project_file_list& files() const;

        void add_file(const project_file& value);

        void machine(hardware::machine* machine);

        void description(const std::string& value);

        void active_environment(project_file* value);

        project_file* find_file(const fs::path& path);

        std::string prop(const std::string& key) const;

        void prop(const std::string& key, const std::string& value);

    protected:
        explicit project(const fs::path& project_path);

    private:
        static void resume_notify();

        static void suspend_notify();

        static void notify_listeners();

    private:
        static bool _notify_enabled;
        static core::project_shared_ptr _instance;
        static std::vector<project_changed_callable> _listeners;

        fs::path _path;
        bool _dirty = false;
        std::string _name {};
        project_file_list _files;
        project_props_dict _props;
        std::string _description {};
        hardware::machine* _machine = nullptr;
        project_file* _active_environment = nullptr;
    };

};

