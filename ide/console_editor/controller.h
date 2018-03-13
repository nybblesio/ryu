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

#include <SDL_events.h>
#include <core/state.h>
#include <ide/ide_types.h>
#include <core/views/caret.h>
#include <core/views/label.h>
#include <core/views/console.h>
#include <core/views/dock_layout_panel.h>

namespace ryu::ide::console_editor {

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
        static const core::code_to_attr_dict _mapper;

        metrics_t _metrics;
        core::console _console;
        bool _show_banner = true;
        core::label _caret_status;
        core::label _machine_status;
        core::label _project_status;
        core::label _document_status;
        core::label _working_directory;
        core::label _environment_status;
        core::dock_layout_panel _header;
        core::dock_layout_panel _footer;
        core::dock_layout_panel _layout_panel;
    };

};
