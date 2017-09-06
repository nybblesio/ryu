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

#include "view.h"

namespace ryu::core {

    class label : public core::view {
    public:
        struct alignment {
            enum types {
                none,
                left,
                right,
                center
            };
        };

        label(
                core::context* context,
                core::view* parent,
                int id,
                const std::string& name);

        std::string value() const;

        alignment::types alignment() const;

        void value(const std::string& value);

        void alignment(alignment::types value);

    protected:
        void on_draw(SDL_Renderer* renderer) override;

    private:
        std::string _value;
        alignment::types _alignment = alignment::types::none;
    };

};

