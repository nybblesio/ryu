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
#include "notification_center.h"

namespace ryu::core {

    namespace fs = boost::filesystem;

    class project : public core::observable {
    public:
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

        static fs::path find_project_root();

        static bool close(core::result& result);

        virtual ~project();

        bool open() const;

        bool dirty() const;

        fs::path path() const;

        void remove_all_files();

        std::string name() const;

        uint32_t id() const override;

        hardware::machine* machine();

        void remove_file(uint32_t id);

        std::string description() const;

        bool save(core::result& result);

        project_file* active_environment();

        void name(const std::string& value);

        project_file* find_file(uint32_t id);

        const project_file_list& files() const;

        observable_type type_id() const override;

        void machine(hardware::machine* machine);

        void description(const std::string& value);

        void active_environment(project_file* value);

        project_file* find_file(const fs::path& path);

        std::string prop(const std::string& key) const;

        void add_file(const project_file_shared_ptr& value);

        void prop(const std::string& key, const std::string& value);

    protected:
        explicit project(const fs::path& project_path);

        void notify() override;

    private:
        void resume_notify();

        void suspend_notify();

    private:
        static core::project_shared_ptr _instance;

        uint32_t _id;
        fs::path _path;
        bool _open = false;
        bool _dirty = false;
        std::string _name {};
        bool _notify_enabled;
        project_file_list _files;
        project_props_dict _props;
        std::string _description {};
        hardware::machine* _machine = nullptr;
        project_file* _active_environment = nullptr;
    };

};

