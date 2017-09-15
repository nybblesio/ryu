//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "selection_view.h"

namespace ryu::ide::machine_editor {

    selection_view::selection_view(
            core::context* context,
            core::view* parent,
            int id,
            const std::string& name) : core::view(context,
                                                  parent,
                                                  core::view::types::custom,
                                                  id,
                                                  name) {
    }

    selection_view::~selection_view() {
    }

    void selection_view::on_draw() {
    }

    bool selection_view::on_process_event(const SDL_Event* e) {
        return false;
    }

}