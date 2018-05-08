//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <core/project.h>
#include <core/project_file.h>
#include "state_header.h"

namespace ryu::core {

    state_header::state_header(
            const std::string& name,
            core::view_host* host) : core::panel(name, host),
                                     _state_label("state-label", host),
                                     _custom_status("custom-status-label", host),
                                     _machine_status("machine-status-label", host),
                                     _project_status("project-status-label", host),
                                     _environment_status("environment-status-label", host) {
    }

    state_header::~state_header() {
        core::notification_center::instance()->remove_observer(id());
    }

    void state_header::on_initialize() {
        _state_label.initialize();
        _state_label.palette(palette());
        _state_label.bg_color(bg_color());
        _state_label.dock(dock::styles::left);
        _state_label.font_family(font_family());
        _state_label.margin({0, font_face()->width, 0, 0 });

        _project_status.initialize();
        _project_status.palette(palette());
        _project_status.fg_color(fg_color());
        _project_status.bg_color(bg_color());
        _project_status.dock(dock::styles::left);
        _project_status.font_family(font_family());
        _project_status.value("| project: (none)");
        _project_status.margin({0, font_face()->width, 0, 0 });

        _environment_status.initialize();
        _environment_status.palette(palette());
        _environment_status.fg_color(fg_color());
        _environment_status.bg_color(bg_color());
        _environment_status.dock(dock::styles::left);
        _environment_status.font_family(font_family());
        _environment_status.value("| env: (none)");
        _environment_status.margin({0, font_face()->width, 0, 0 });

        _machine_status.initialize();
        _machine_status.palette(palette());
        _machine_status.fg_color(fg_color());
        _machine_status.bg_color(bg_color());
        _machine_status.dock(dock::styles::left);
        _machine_status.font_family(font_family());
        _machine_status.value("| machine: (none)");
        _machine_status.margin({0, font_face()->width, 0, 0 });

        _custom_status.initialize();
        _custom_status.palette(palette());
        _custom_status.fg_color(fg_color());
        _custom_status.bg_color(bg_color());
        _custom_status.font_family(font_family());
        _custom_status.dock(dock::styles::left);
        _custom_status.margin({0, font_face()->width, 0, 0 });

        add_child(&_state_label);
        add_child(&_project_status);
        add_child(&_environment_status);
        add_child(&_machine_status);
        add_child(&_custom_status);

        core::notification_center::instance()->add_observer(id(), this);
    }

    std::string state_header::state() const {
        return _state_label.value();
    }

    std::string state_header::custom() const {
        return _custom_status.value();
    }

    palette_index state_header::state_color() const {
        return _state_label.fg_color();
    }

    void state_header::state(const std::string& name) {
        _state_label.value(name);
    }

    void state_header::state_color(palette_index color) {
        _state_label.fg_color(color);
    }

    void state_header::custom(const std::string& value) {
        _custom_status.value(value);
    }

    void state_header::on_notification(core::observable* instance) {
        auto project = dynamic_cast<core::project*>(instance);

        std::string project_name = "(none)";
        std::string machine_name = "(none)";
        std::string environment_name = "(none)";

        if (project->open()) {
            project_name = project->name();
            if (project->dirty())
                project_name += "*";
            if (project->machine() != nullptr) {
                machine_name = core::project::instance()->machine()->name();
            }
            auto active_environment = project->active_environment();
            if (active_environment != nullptr) {
                environment_name = active_environment->name();
            }
        }

        _project_status.value(fmt::format("| project: {}", project_name));
        _machine_status.value(fmt::format("| machine: {}", machine_name));
        _environment_status.value(fmt::format("| env: {}", environment_name));
    }

    bool state_header::on_notification_filter(core::observable* instance) {
        return instance->type_id() == observables::types::project;
    }

}