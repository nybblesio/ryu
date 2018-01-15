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

    // N.B. ideas behind environments:
    //  - std::vector<boost::filesystem::path> for each environment assembly file
    //  - commands in command parser to switch between environments
    //  -       this does a environment::load (which does an assemble/execute)
    //  - command for newenv
    //  - command for switchenv
    //  - command for delenv
    //  - command for listenv
    //
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

        std::string name() const;

        project_file_list& files();

        hardware::machine* machine();

        std::string description() const;

        bool save(core::result& result);

        void name(const std::string& value);

        void machine(hardware::machine* machine);

        void description(const std::string& value);

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
        std::string _description {};
        hardware::machine* _machine = nullptr;
    };

};

