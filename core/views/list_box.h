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

    class list_box  : public core::view {
    public:
        using on_clicked_callable = std::function<void ()>;

        list_box(
            const std::string& name,
            core::view_host* host);

        ~list_box() override;

        border::types border() const;

        void border(border::types value);

        void on_clicked(const on_clicked_callable& callable);

    protected:
        void bind_events();

        void define_actions();

        void on_initialize() override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const rect& context_bounds) override;

    private:
        on_clicked_callable _on_clicked {};
        border::types _border = border::types::solid;
    };

};

