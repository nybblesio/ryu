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

        button(
            const std::string& name,
            core::view_host* host);

        ~button() override;

        int width() const;

        int height() const;

        void width(int value);

        void height(int value);

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
        void bind_events();

        void define_actions();

        void on_initialize() override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const rect& context_bounds) override;

    private:
        int _width = 120;
        int _height = 50;
        std::string _value;
        on_clicked_callable _on_clicked {};
        border::types _border = border::types::solid;
        alignment::vertical::types _valign = alignment::vertical::middle;
        alignment::horizontal::types _halign = alignment::horizontal::center;
    };

};

