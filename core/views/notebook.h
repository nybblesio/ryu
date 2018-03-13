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

    class notebook : public core::view {
    public:
        notebook(
            const std::string& name,
            core::view_host* host);

        int active_tab() const;

        void remove_tab(int index);

        void active_tab(int index);

        core::rect client_bounds() override;

        void add_tab(const std::string& title, core::view* content);

    protected:
        // XXX: this should not be hardcoded like this
        const int tab_width = 200;
        const int tab_height = 50;

        void on_draw(core::renderer& surface) override;

        void draw_children(core::renderer& surface) override;

        void on_resize(const core::rect& context_bounds) override;

    private:
        int _index = 0;
        std::vector<std::string> _tabs;
    };

};

