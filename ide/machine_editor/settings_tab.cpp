//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <fmt/format.h>
#include "settings_tab.h"

namespace ryu::ide::machine_editor {

    settings_tab::settings_tab(
            const std::string& name,
            core::view_host* host) : core::view(core::view::types::container, name, host) {
    }

    void settings_tab::initialize() {
    }

    void settings_tab::on_draw(core::renderer& surface) {
    }

}