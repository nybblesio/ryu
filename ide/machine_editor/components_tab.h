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

#include <core/view.h>

namespace ryu::ide::machine_editor {

    class components_tab : public core::view {
    public:
        components_tab(
            const std::string& name,
            core::view_host* host);

        void initialize();

    protected:
        void on_draw(core::renderer& surface) override;
    };

};

