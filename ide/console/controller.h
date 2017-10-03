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
#include <core/views/caret.h>
#include <core/views/label.h>
#include <core/views/dock_layout_panel.h>
#include "console_view.h"

namespace ryu::ide::console {

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

        bool on_process_event(const SDL_Event* e) override;

        void on_activate(const core::parameter_dict& params) override;

    private:
        metrics_t _metrics;
        core::label _header;
        core::label _footer;
        bool _show_banner = true;
        console::console_view _console;
        core::dock_layout_panel _layout_panel;
    };

};
