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

#include "dock_layout_panel.h"

namespace ryu::core {

    class loadable_view : public core::dock_layout_panel {
    public:
        loadable_view(
            const std::string& name,
            core::view_host* host);

        ~loadable_view() override;

        bool load(
            core::result& result,
            const boost::filesystem::path& path);

    protected:
        void on_initialize() override;

    private:
    };

};

