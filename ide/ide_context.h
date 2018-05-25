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

#include <core/context.h>
#include <core/project.h>
#include <core/environment.h>
#include <ide/hex_editor/controller.h>
#include <ide/tile_editor/controller.h>
#include <ide/actor_editor/controller.h>
#include <ide/machine_list/controller.h>
#include <ide/sprite_editor/controller.h>
#include <ide/source_editor/controller.h>
#include <ide/sample_editor/controller.h>
#include <ide/module_editor/controller.h>
#include <ide/console_editor/controller.h>
#include <ide/machine_editor/controller.h>
#include <ide/project_editor/controller.h>
#include <ide/palette_editor/controller.h>
#include <ide/component_editor/controller.h>
#include <ide/background_editor/controller.h>
#include <ide/preferences_editor/controller.h>

namespace ryu::ide {

    class ide_context : public core::context {
    public:
        explicit ide_context(const std::string& name);

        core::context_window::sizes size() const;

        void size(core::context_window::sizes value);

        void parent_resize(const core::rect& parent_bounds);

    protected:
        void on_draw(core::renderer& surface) override;

        bool on_initialize(core::result& result) override;

    private:
        void bind_events();

        void define_actions();

        void configure_palette();

        bool execute_transition_command(
            core::state_transition_command command,
            const core::parameter_dict& params);

        void configure_states(core::result& result);

    private:
        static std::map<core::state_transition_command, uint32_t> s_state_transitions;

        hex_editor::controller _hex_editor_state;
        console_editor::controller _console_state;
        tile_editor::controller _tile_editor_state;
        machine_list::controller _machine_list_state;
        actor_editor::controller _actor_editor_state;
        sprite_editor::controller _sprite_editor_state;
        source_editor::controller _source_editor_state;
        module_editor::controller _module_editor_state;
        sample_editor::controller _sample_editor_state;
        palette_editor::controller _palette_editor_state;
        machine_editor::controller _machine_editor_state;
        project_editor::controller _project_editor_state;
        component_editor::controller _component_editor_state;
        background_editor::controller _background_editor_state;
        preferences_editor::controller _preferences_editor_state;
        core::context_window::sizes _size = core::context_window::sizes::split;
    };

};