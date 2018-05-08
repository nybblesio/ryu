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

#include "panel.h"

namespace ryu::core {

    class palette_editor : public core::panel {
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

    private:
        void rebuild_entries();

    private:
        std::vector<std::unique_ptr<view>> _views {};
    };

};