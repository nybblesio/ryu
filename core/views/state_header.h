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

#include "label.h"
#include "dock_layout_panel.h"

namespace ryu::core {

    class state_header : public core::dock_layout_panel {
    public:
        state_header(
            const std::string& name,
            core::view_host* host);

        ~state_header() override;

        std::string state() const;

        std::string custom() const;

        uint8_t state_color() const;

        void state_color(uint8_t color);

        void state(const std::string& name);

        void custom(const std::string& value);

    protected:
        void on_initialize() override;

        void on_resize(const core::rect& context_bounds) override;

    private:
        core::label _state_label;
        core::label _custom_status;
        core::label _machine_status;
        core::label _project_status;
        core::label _environment_status;
    };

};

