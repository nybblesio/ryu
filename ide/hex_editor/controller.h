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
#include <core/view_factory.h>

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

        void on_deactivate() override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const core::rect& bounds) override;

        void on_activate(const core::parameter_dict& params) override;

        void on_update(uint32_t dt, core::pending_event_list& events) override;

    private:
        void bind_events();

        void define_actions();

    private:
        metrics_t _metrics;
        core::label_unique_ptr _caret_status;
        core::state_header_unique_ptr _header;
        core::textbox_unique_ptr _command_line;
        core::memory_editor_unique_ptr _editor;
        core::dock_layout_panel_unique_ptr _footer;
        core::dock_layout_panel_unique_ptr _layout_panel;
    };

};

