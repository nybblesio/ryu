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

    class button : public core::view {
    public:
        using on_clicked_callable = std::function<void ()>;

        button(core::context* context, const std::string& name);

        std::string value() const;

        border::types border() const;

        void border(border::types value);

        void value(const std::string& value);

        alignment::vertical::types valign() const;

        alignment::horizontal::types halign() const;

        void valign(alignment::vertical::types value);

        void halign(alignment::horizontal::types value);

        void on_clicked(const on_clicked_callable& callable);

    protected:
        void on_draw() override;

        bool on_process_event(const SDL_Event* e) override;

    private:
        std::string _value;
        on_clicked_callable _on_clicked {};
        border::types _border = border::types::solid;
        alignment::vertical::types _valign = alignment::vertical::middle;
        alignment::horizontal::types _halign = alignment::horizontal::center;
    };

};

