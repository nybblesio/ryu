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
#include <core/view_factory.h>
#include <core/views/console.h>
#include <core/views/state_header.h>

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
        static const core::code_to_attr_dict s_mapper;

        metrics_t _metrics;
        bool _show_banner = true;
        core::console_unique_ptr _console;
        core::label_unique_ptr _caret_status;
        core::state_header_unique_ptr _header;
        core::label_unique_ptr _document_status;
        core::dock_layout_panel_unique_ptr _footer;
        core::dock_layout_panel_unique_ptr _layout_panel;
    };

};
