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

namespace ryu::core {

    class panel : public core::view {
    public:
        panel(
            const std::string& name,
            core::view_host* host);

        ~panel() override;

        core::border::types border() const;

        void border(core::border::types value);

    protected:
        void on_initialize() override;

        void on_draw(core::renderer& surface) override;

    private:
        core::border::types _border = core::border::types::none;
    };

};

