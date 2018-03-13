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

#include "panel.h"

namespace ryu::core {

    class dock_layout_panel : public core::panel {
    public:
        dock_layout_panel(
            const std::string& name,
            core::view_host* host);

    protected:
        void on_resize(const core::rect& context_bounds) override;
    };

};

