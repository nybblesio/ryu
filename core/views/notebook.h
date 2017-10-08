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

    struct tab_t {
        std::string title;
        core::view* content = nullptr;
    };

    class notebook : public core::view {
    public:
        explicit notebook(const std::string& name);

        int active_tab() const;

        void remove_tab(int index);

        void active_tab(int index);

        void add_tab(const std::string& title, core::view* content);

    protected:
        void on_draw(core::renderer& surface) override;

        bool on_process_event(const SDL_Event* e) override;

        void on_resize(const core::rect& context_bounds) override;

    private:
        int _index = 0;
        std::vector<tab_t> _tabs;
    };

};

