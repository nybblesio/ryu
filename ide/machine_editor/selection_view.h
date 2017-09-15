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

#include <string>
#include <core/view.h>
#include <core/views/label.h>
#include <core/views/textbox.h>

namespace ryu::ide::machine_editor {

    class selection_view : public core::view {
    public:
        enum ids {
        };

        selection_view(
                core::context* context,
                core::view* parent,
                int id,
                const std::string& name);

        ~selection_view() override;

    protected:
        void on_draw() override;

        bool on_process_event(const SDL_Event* e) override;

    private:
    };

};

