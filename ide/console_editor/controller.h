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
#include <ide/ide_types.h>
#include <core/view_factory.h>

namespace ryu::ide::console_editor {

    class controller : public core::state {
    public:
        explicit controller(const std::string& name);

    protected:
        void on_initialize() override;

        bool on_load(core::result& result) override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const core::rect& bounds) override;

        void on_activate(const core::parameter_dict& params) override;

        void on_update(uint32_t dt, core::pending_event_list& events) override;

    private:
        static const core::code_to_attr_dict s_mapper;

        core::console* _console;
        bool _show_banner = true;
        core::document_footer* _footer;
        core::dock_layout_panel_unique_ptr _layout_panel;
    };

};
