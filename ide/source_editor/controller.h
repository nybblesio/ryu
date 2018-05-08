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
        core::text_editor* _editor;
        core::state_header* _header;
        core::text_box* _command_line;
        core::document_footer* _footer;
        core::loadable_view_unique_ptr _layout_panel;
    };

};
