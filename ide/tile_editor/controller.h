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

namespace ryu::ide::tile_editor {

    class controller : public ryu::core::state {
    public:
        explicit controller(const std::string& name);

    protected:
        void on_deactivate() override;

        void on_initialize() override;

        bool on_load(core::result& result) override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const core::rect& bounds) override;

        void on_activate(const core::parameter_dict& params) override;

        void on_update(uint32_t dt, core::pending_event_list& events) override;

    private:
        void bind_events();

        void update_values();

        void define_actions();

    private:
        core::loadable_view_unique_ptr _layout_panel;
    };

};

