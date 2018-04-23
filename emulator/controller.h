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

namespace ryu::emulator {

    class controller  : public core::state {
    public:
        explicit controller(const std::string& name);

        ~controller() override;

    protected:
        void on_initialize() override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const core::rect& bounds) override;

        void on_update(uint32_t dt, core::pending_event_list& events) override;

    private:
    };

};

