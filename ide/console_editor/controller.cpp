//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <sstream>
#include <iostream>
#include <fmt/format.h>
#include <core/engine.h>
#include <core/context.h>
#include <ide/ide_types.h>
#include <core/environment.h>
#include <core/project_file.h>
#include "controller.h"

namespace ryu::ide::console_editor {

    const core::code_to_attr_dict controller::s_mapper = {
        {"bold",        [](core::attr_t& attr) { attr.style |= core::font::styles::bold; }},
        {"italic",      [](core::attr_t& attr) { attr.style |= core::font::styles::italic; }},
        {"underline",   [](core::attr_t& attr) { attr.style |= core::font::styles::underline; }},
        {"rev",         [](core::attr_t& attr) { attr.flags |= core::font::flags::reverse; }},
        {"black",       [](core::attr_t& attr) { attr.color = ide::colors::black; }},
        {"white",       [](core::attr_t& attr) { attr.color = ide::colors::white; }},
        {"red",         [](core::attr_t& attr) { attr.color = ide::colors::red; }},
        {"cyan",        [](core::attr_t& attr) { attr.color = ide::colors::cyan; }},
        {"purple",      [](core::attr_t& attr) { attr.color = ide::colors::purple; }},
        {"green",       [](core::attr_t& attr) { attr.color = ide::colors::green; }},
        {"blue",        [](core::attr_t& attr) { attr.color = ide::colors::blue; }},
        {"yellow",      [](core::attr_t& attr) { attr.color = ide::colors::yellow; }},
        {"orange",      [](core::attr_t& attr) { attr.color = ide::colors::orange; }},
        {"brown",       [](core::attr_t& attr) { attr.color = ide::colors::brown; }},
        {"pink",        [](core::attr_t& attr) { attr.color = ide::colors::pink; }},
        {"dgrey",       [](core::attr_t& attr) { attr.color = ide::colors::dark_grey; }},
        {"grey",        [](core::attr_t& attr) { attr.color = ide::colors::grey; }},
        {"lgreen",      [](core::attr_t& attr) { attr.color = ide::colors::light_green; }},
        {"lblue",       [](core::attr_t& attr) { attr.color = ide::colors::light_blue; }},
        {"lgrey",       [](core::attr_t& attr) { attr.color = ide::colors::light_grey; }},
    };

    controller::controller(const std::string& name) : ryu::core::state(name) {
    }

    void controller::on_initialize() {
        _project_status = core::view_factory::create_label(
            this,
            "project-status-label",
            ide::colors::info_text,
            ide::colors::fill_color,
            "project: (none)",
            core::dock::styles::left,
            {0, context()->font_face()->width, 0, 0});

        _machine_status = core::view_factory::create_label(
            this,
            "machine-status-label",
            ide::colors::info_text,
            ide::colors::fill_color,
            "| machine: (none)",
            core::dock::styles::left,
            {0, context()->font_face()->width, 0, 0});

        _working_directory = core::view_factory::create_label(
            this,
            "working-directory-label",
            ide::colors::info_text,
            ide::colors::fill_color,
            fmt::format("| cwd: {}", boost::filesystem::current_path().string()),
            core::dock::styles::left);

        _header = core::view_factory::create_dock_layout_panel(
            this,
            "header-panel",
            ide::colors::info_text,
            ide::colors::fill_color,
            core::dock::styles::top,
            {_metrics.left_padding, _metrics.right_padding, 5, 0});
        _header->bounds().height(context()->font_face()->line_height);
        _header->add_child(_project_status.get());
        _header->add_child(_machine_status.get());
        _header->add_child(_working_directory.get());

        core::project::add_listener([&]() {
            std::string project_name = "(none)";
            std::string machine_name = "(none)";
            if (core::project::instance() != nullptr) {
                project_name = core::project::instance()->name();
                if (core::project::instance()->dirty())
                    project_name += "*";
                if (core::project::instance()->machine() != nullptr) {
                    machine_name = core::project::instance()->machine()->name();
                }
            }
            _project_status->value(fmt::format("project: {}", project_name));
            _machine_status->value(fmt::format(" | machine: {}", machine_name));
        });

        _document_status = core::view_factory::create_label(
            this,
            "document-status-label",
            ide::colors::info_text,
            ide::colors::fill_color,
            "",
            core::dock::styles::left,
            {0, context()->font_face()->width, 0, 0});

        _caret_status = core::view_factory::create_label(
            this,
            "caret-status-label",
            ide::colors::info_text,
            ide::colors::fill_color);

        _environment_status = core::view_factory::create_label(
            this,
            "environment-status-label",
            ide::colors::info_text,
            ide::colors::fill_color,
            fmt::format(
                " | env: {}",
                context()->environment()->name()));

        _footer = core::view_factory::create_dock_layout_panel(
            this,
            "footer-panel",
            ide::colors::info_text,
            ide::colors::fill_color,
            core::dock::styles::bottom,
            {_metrics.left_padding, _metrics.right_padding, 5, 5});
        _footer->bounds().height(context()->font_face()->line_height);
        _footer->add_child(_document_status.get());
        _footer->add_child(_caret_status.get());
        _footer->add_child(_environment_status.get());

        _console = core::view_factory::create_console(
            this,
            "console",
            ide::colors::text,
            ide::colors::fill_color,
            s_mapper);
        _console->caret_color(ide::colors::caret);
        _console->on_transition([&](const std::string& name, const core::parameter_dict& params) {
            return transition_to(name, params);
        });
        _console->on_caret_changed([&](const core::caret& caret, const core::document& document) {
            _document_status->value(fmt::format(
                    "C:{:03d}/{:03d} R:{:04d}/{:04d}",
                    document.column() + caret.column() + 1,
                    document.columns(),
                    document.row() + caret.row() + 1,
                    document.rows()));
            _caret_status->value(fmt::format(
                    "| X:{:03d} Y:{:02d} | {}",
                    caret.column() + 1,
                    caret.row() + 1,
                    caret.mode() == core::caret::mode::overwrite ? "OVR" : "INS"));
        });
        _console->on_execute_command([&](core::result& result, const std::string& input) {
            auto success = context()->environment()->execute(result, input);
            if (success) {
                auto command_action_msg = result.find_code("command_action");
                if (command_action_msg == nullptr)
                    return success;

                auto command = command_action_msg->get_parameter<std::string>("action");
                if (command == "quit") {
                    context()->engine()->quit();
                } else if (command == "update_working_directory") {
                    _working_directory->value(fmt::format(
                            "| cwd: {}",
                            boost::filesystem::current_path().string()));
                }
            }
            return success;
        });
        _console->focus(_console.get());

        _layout_panel = core::view_factory::create_dock_layout_panel(
            this,
            "layout-panel",
            ide::colors::info_text,
            ide::colors::fill_color);
        _layout_panel->add_child(_header.get());
        _layout_panel->add_child(_footer.get());
        _layout_panel->add_child(_console.get());

        core::project::add_listener([&](){
            std::string name = "(none)";
            auto project = core::project::instance();
            if (project != nullptr) {
                auto active_environment = project->active_environment();
                if (active_environment != nullptr) {
                    name = active_environment->name();
                }
            }
            _environment_status->value(fmt::format(" | env: {}", name));
        });
    }

    void controller::on_update(uint32_t dt) {
        _console->update(dt);
    }

    void controller::on_draw(core::renderer& surface) {
        _layout_panel->draw(surface);
    }

    void controller::on_resize(const core::rect& bounds) {
        _layout_panel->resize(bounds);
    }

    void controller::on_activate(const core::parameter_dict& params) {
        if (!_show_banner)
            return;

        _console->write_message("<rev> <bold>Ryu: <italic>The Arcade Construction Kit <>");
        _console->write_message(" Copyright (C) 2017 Jeff Panici");
        _console->write_message(" See details in <underline><bold>LICENSE<> file");
        _console->caret_down();
        _console->write_message("Ready.");
        _show_banner = false;
    }
}