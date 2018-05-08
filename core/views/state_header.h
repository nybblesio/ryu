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

#include <core/notification_center.h>
#include "label.h"
#include "panel.h"

namespace ryu::core {

    class state_header : public core::panel,
                         public core::observer {
    public:
        state_header(
            const std::string& name,
            core::view_host* host);

        ~state_header() override;

        std::string state() const;

        std::string custom() const;

        palette_index state_color() const;

        void state(const std::string& name);

        void state_color(palette_index color);

        void custom(const std::string& value);

    protected:
        void on_initialize() override;

        void on_notification(core::observable* instance) override;

        bool on_notification_filter(core::observable* instance) override;

    private:
        core::label _state_label;
        core::label _custom_status;
        core::label _machine_status;
        core::label _project_status;
        core::label _environment_status;
    };

};

