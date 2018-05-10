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

    class integrated_circuit_editor  : public core::view {
    public:
        integrated_circuit_editor(
            const std::string& name,
            core::view_host* host);

        ~integrated_circuit_editor() override;

    protected:
        void on_initialize() override;

        void on_font_family_changed() override;

        void on_draw(core::renderer& surface) override;

    private:
        void update_minimum_size();
    };

};