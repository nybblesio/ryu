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

    class label : public core::view {
    public:
        explicit label(const std::string& name);

        std::string value() const;

        border::types border() const;

        void border(border::types value);

        void value(const std::string& value);

        alignment::vertical::types valign() const;

        alignment::horizontal::types halign() const;

        void valign(alignment::vertical::types value);

        void halign(alignment::horizontal::types value);

    protected:
        void on_draw(SDL_Renderer* renderer) override;

    private:
        std::string _value;
        border::types _border = border::types::none;
        alignment::vertical::types _valign = alignment::vertical::middle;
        alignment::horizontal::types _halign = alignment::horizontal::none;
    };

};

