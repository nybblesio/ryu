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
#include <core/view_factory.h>

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

        void on_activate(const core::parameter_dict& params) override;

    private:
        void bind_events();

    private:
        metrics_t _metrics;
        core::text_editor_unique_ptr _editor;
        core::state_header_unique_ptr _header;
        core::textbox_unique_ptr _command_line;
        core::document_footer_unique_ptr _footer;
        core::dock_layout_panel_unique_ptr _layout_panel;
    };

};
