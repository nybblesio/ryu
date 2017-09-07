//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "editor_view.h"

namespace ryu::ide::machine_editor {

    editor_view::editor_view(
            core::context* context,
            core::view* parent,
            int id,
            const std::string& name) : core::view(context, parent, core::view::types::custom, id, name),
                                       _name_label(context, this, ids::name_label, "name-label"),
                                       _name_textbox(context, this, ids::name_textbox, "name-textbox") {
    }

    editor_view::~editor_view() {
    }

    void editor_view::on_draw(SDL_Renderer* renderer) {
    }

    bool editor_view::on_process_event(const SDL_Event* e) {
        return false;
    }

}