//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <core/state.h>
#include <core/project.h>
#include <hardware/machine.h>
#include <core/views/label.h>
#include <core/views/textbox.h>
#include <common/SDL_FontCache.h>
#include <core/views/dock_layout_panel.h>
#include "editor_view.h"

namespace ryu::ide::text_editor {

    class controller : public ryu::core::state {
    public:
        const uint32_t rows = 4096;
        const uint16_t columns = 128;

        explicit controller(const std::string& name);

        core::project* project();

        hardware::machine* machine();

        void project(core::project* value);

        void machine(hardware::machine* value);

    protected:
        struct metrics_t {
            const int left_padding = 10;
            const int right_padding = 10;
        };

        void on_initialize() override;

        void on_update(uint32_t dt) override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const core::rect& bounds) override;

        bool on_process_event(const SDL_Event* e) override;

    private:
        metrics_t _metrics;
        editor_view _editor;
        core::label _cpu_status;
        core::label _file_status;
        core::label _caret_status;
        core::textbox _command_line;
        core::label _document_status;
        core::dock_layout_panel _footer;
        core::dock_layout_panel _header;
        core::project* _project = nullptr;
        hardware::machine* _machine = nullptr;
        core::dock_layout_panel _layout_panel;
    };

};
