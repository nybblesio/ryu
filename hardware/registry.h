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
#include <boost/filesystem.hpp>
#include <hardware/hardware_types.h>

namespace ryu::hardware {

    namespace fs = boost::filesystem;

    class registry {
    public:
        static registry* instance();

        machine_list machines();

        display_list& displays() const;

        hardware::machine* new_machine();

        void remove_machine(uint32_t id);

        hardware::machine* find_machine(uint32_t id);

        hardware::display* find_display(uint32_t id);

        ic_type_list integrated_circuit_types() const;

        bool save(core::result& result, const fs::path& path);

        bool load(core::result& result, const fs::path& path);

        hardware::machine* find_machine(const std::string& name);

        hardware::integrated_circuit* new_ic_by_type_id(uint32_t type_id);

    protected:
        registry() = default;

        virtual ~registry() = default;

    private:
        machine_dict _machines;
    };

};

