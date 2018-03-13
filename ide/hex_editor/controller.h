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

#include <core/state.h>
#include <core/views/label.h>
#include <core/views/textbox.h>
#include <core/views/memory_editor.h>
#include <core/views/dock_layout_panel.h>

namespace ryu::ide::hex_editor {

    class controller : public core::state {
    public:
        explicit controller(const std::string& name);

    protected:
        struct metrics_t {
            const int left_padding = 10;
            const int right_padding = 10;
        };

        void on_initialize() override;

        void on_update(uint32_t dt) override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const core::rect& bounds) override;

        void on_activate(const core::parameter_dict& params) override;

    private:
        metrics_t _metrics;
        core::label _caret_status;
        core::label _machine_status;
        core::label _project_status;
        core::textbox _command_line;
        core::memory_editor _editor;
        core::label _environment_status;
        core::dock_layout_panel _header;
        core::dock_layout_panel _footer;
        core::dock_layout_panel _layout_panel;
    };

};

