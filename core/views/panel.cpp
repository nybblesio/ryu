//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "panel.h"

namespace ryu::core {

    panel::panel(
            core::context* context,
            core::view* parent,
            int id,
            const std::string& name) : core::view(context, parent, types::custom, id, name) {
    }

    void panel::on_draw() {
        auto& bg = (*context()->palette())[bg_color()];
        set_color(bg);
        fill_rect(client_rect());
    }
}