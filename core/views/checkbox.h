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

    class checkbox : public core::view {
    public:
        checkbox(
            const std::string& name,
            core::view_host* host);

        bool value() const;

        void value(bool flag);

    protected:
        void on_draw(core::renderer& surface) override;

    private:
        bool _value;
    };

};

