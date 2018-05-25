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

    class check_box : public core::view {
    public:
        check_box(
            const std::string& name,
            core::view_host* host);

        ~check_box() override;

    protected:
        void on_initialize() override;

        void on_draw(core::renderer& surface) override;

    private:
        void bind_events();

        void define_actions();
    };

};

