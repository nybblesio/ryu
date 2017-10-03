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

#include <core/result.h>
#include <core/project.h>
#include <core/views/label.h>
#include <core/views/panel.h>
#include <hardware/machine.h>
#include <core/views/button.h>
#include <core/views/textbox.h>
#include <core/views/pick_list.h>

namespace ryu::ide::machine_editor {

    class editor_view : public core::view {
    public:
        explicit editor_view(const std::string& name);

        void initialize();

    protected:
        void on_draw(core::renderer& surface) override;

        bool on_process_event(const SDL_Event* e) override;
    };

};

