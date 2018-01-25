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
#include "components_tab.h"

namespace ryu::ide::machine_editor {

    components_tab::components_tab(const std::string& name) : core::view(core::view::types::container, name) {
    }

    void components_tab::initialize() {
    }

    void components_tab::on_draw(core::renderer& surface) {
    }

    bool components_tab::on_process_event(const SDL_Event* e) {
        return false;
    }

}