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

    class panel  : public core::view {
    public:
        panel(core::context* context, const std::string& name);

        border::types border() const;

        void border(border::types value);

    protected:
        void on_draw() override;

    private:
        border::types _border = border::types::none;
    };

};

