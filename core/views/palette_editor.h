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

    class palette_editor : public core::view {
    public:
        palette_editor(
            const std::string& name,
            core::view_host* host);

        ~palette_editor() override;

    protected:
        void bind_events();

        void define_actions();

        void on_initialize() override;

        void on_focus_changed() override;

        void on_palette_changed() override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const rect& context_bounds) override;

    private:
        void rebuild_entries();

    private:
        std::vector<std::unique_ptr<view>> _views {};
    };

};