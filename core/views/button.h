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
                core::context* context,
                core::view* parent,
                int id,
                const std::string& name);

        ~button() override;

        std::string value() const;

        alignment::types alignment() const;

        void value(const std::string& value);

        void alignment(alignment::types value);

        void on_clicked(const on_clicked_callable& callable);

    protected:
        void on_draw() override;

        bool on_process_event(const SDL_Event* e) override;

    private:
        std::string _value;
        on_clicked_callable _on_clicked;
        alignment::types _alignment = alignment::types::none;
    };

};

