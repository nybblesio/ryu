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
        void on_initialize() override;

        void on_deactivate() override;

        bool on_load(core::result& result) override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const core::rect& bounds) override;

        void on_activate(const core::parameter_dict& params) override;

        void on_update(uint32_t dt, core::pending_event_list& events) override;

    private:
        void bind_events();

        void define_actions();

    private:
        uint32_t _address = 0;
        core::text_box* _command_line;
        core::memory_editor* _memory_editor;
        core::loadable_view_unique_ptr _layout_panel;
    };

};

