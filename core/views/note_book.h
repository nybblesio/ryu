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

    class note_book : public core::view {
    public:
        note_book(
            const std::string& name,
            core::view_host* host);

        ~note_book() override;

        size_t active_tab() const;

        void remove_tab(size_t index);

        void active_tab(size_t index);

        void add_tab(const std::string& title, core::view* content);

    protected:
        void on_initialize() override;

        void on_draw(core::renderer& surface) override;

    private:
        void bind_events();

        void define_actions();

    private:
        size_t _index = 0;
        std::vector<std::string> _tabs;
    };

};

