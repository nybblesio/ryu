//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "integrated_circuit_editor.h"

namespace ryu::core {

    integrated_circuit_editor::integrated_circuit_editor(
        const std::string& name,
        core::view_host* host) : core::view(types::container, name, host) {
    }

    integrated_circuit_editor::~integrated_circuit_editor() {
    }

    void integrated_circuit_editor::on_initialize() {
        should_clip(true);
        update_minimum_size();
    }

    void integrated_circuit_editor::update_minimum_size() {
        auto& minimum_size = min_size();
        minimum_size.dimensions(256, 256);
    }

    void integrated_circuit_editor::on_font_family_changed() {
        update_minimum_size();
    }

    void integrated_circuit_editor::on_draw(core::renderer& surface) {
    }

}