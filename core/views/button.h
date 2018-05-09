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

        std::string shortcut() const;

        palette_index shortcut_color() const;

        void shortcut(const std::string& value);

        void shortcut_color(palette_index value);

        alignment::vertical::types valign() const;

        alignment::horizontal::types halign() const;

        void valign(alignment::vertical::types value);

        void halign(alignment::horizontal::types value);

        void on_clicked(const on_clicked_callable& callable);

    protected:
        void bind_events();

        void define_actions();

        void on_initialize() override;

        void on_font_family_changed() override;

        void on_draw(core::renderer& surface) override;

    private:
        void update_minimum_size();

    private:
        std::string _shortcut;
        int _shortcut_text_width = 0;
        palette_index _shortcut_color;
        on_clicked_callable _on_clicked {};
        alignment::vertical::types _valign = alignment::vertical::middle;
        alignment::horizontal::types _halign = alignment::horizontal::center;
    };

};

