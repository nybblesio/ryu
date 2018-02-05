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
#include <core/views/text_editor.h>
#include <core/views/dock_layout_panel.h>

namespace ryu::ide::source_editor {

    class controller : public ryu::core::state {
    public:
        const uint32_t rows = 4096;
        const uint16_t columns = 128;

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

        bool on_process_event(const SDL_Event* e) override;

        void on_activate(const core::parameter_dict& params) override;

    private:
        metrics_t _metrics;
        core::label _cpu_status;
        core::label _file_status;
        core::label _caret_status;
        core::text_editor _editor;
        core::label _project_label;
        core::label _machine_label;
        core::textbox _command_line;
        core::label _document_status;
        core::dock_layout_panel _footer;
        core::dock_layout_panel _header;
        core::dock_layout_panel _layout_panel;
    };

};
