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
#include "controller.h"

namespace ryu::ide::console_editor {

    const core::code_to_attr_dict controller::_mapper = {
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

    controller::controller(const std::string& name) : ryu::core::state(name),
                                                      _header("header-label"),
                                                      _console("console"),
                                                      _caret_status("caret-status"),
                                                      _document_status("document-status"),
                                                      _working_directory("working-directory"),
                                                      _footer("footer-label"),
                                                      _layout_panel("layout-panel") {
    }

    void controller::on_initialize() {
        auto family = context()->engine()->find_font_family("hack");
        auto face = family->find_style(core::font::styles::normal);

        _header.font_family(family);
        _header.palette(context()->palette());
        _header.dock(core::dock::styles::top);
        _header.fg_color(ide::colors::info_text);
        _header.bg_color(ide::colors::fill_color);
        _header.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
        _header.value("project: (none) | machine: (none)");

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
            _header.value(fmt::format("project: {} | machine: {}", project_name, machine_name));
        });

        _document_status.font_family(family);
        _document_status.palette(context()->palette());
        _document_status.dock(core::dock::styles::left);
        _document_status.fg_color(ide::colors::info_text);
        _document_status.bg_color(ide::colors::fill_color);
        _document_status.margin({0, face->width, 0, 0});

        _caret_status.font_family(family);
        _caret_status.margin({0, 0, 0, 0});
        _caret_status.palette(context()->palette());
        _caret_status.dock(core::dock::styles::left);
        _caret_status.fg_color(ide::colors::info_text);
        _caret_status.bg_color(ide::colors::fill_color);

        _working_directory.font_family(family);
        _working_directory.margin({0, 0, 0, 0});
        _working_directory.palette(context()->palette());
        _working_directory.dock(core::dock::styles::left);
        _working_directory.fg_color(ide::colors::info_text);
        _working_directory.bg_color(ide::colors::fill_color);
        _working_directory.value(fmt::format(" | {}", boost::filesystem::current_path().string()));

        _footer.font_family(family);
        _footer.palette(context()->palette());
        _footer.dock(core::dock::styles::bottom);
        _footer.bounds().height(face->line_height);
        _footer.fg_color(ide::colors::info_text);
        _footer.bg_color(ide::colors::fill_color);
        _footer.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
        _footer.add_child(&_document_status);
        _footer.add_child(&_caret_status);
        _footer.add_child(&_working_directory);

        _console.font_family(family);
        _console.code_mapper(_mapper);
        _console.fg_color(ide::colors::text);
        _console.palette(context()->palette());
        _console.dock(core::dock::styles::fill);
        _console.caret_color(ide::colors::caret);
        _console.bg_color(ide::colors::fill_color);
        _console.on_transition([&](const std::string& name, const core::parameter_dict& params) {
            return transition_to(name, params);
        });
        _console.on_caret_changed([&](const core::caret& caret, const core::document& document) {
            _document_status.value(fmt::format(
                    "R:{:04d}/{:04d}",
                    document.row() + caret.row() + 1,
                    document.rows()));
            _caret_status.value(fmt::format(
                    "| X:{:03d} Y:{:02d} | {}",
                    caret.column() + 1,
                    caret.row() + 1,
                    caret.mode() == core::caret::mode::overwrite ? "OVR" : "INS"));
        });

        _console.on_execute_command([&](core::result& result, const std::string& input) {
            auto success = context()->environment()->execute(result, input);
            if (success) {
                auto command_action_msg = result.find_code("command_action");
                if (command_action_msg == nullptr)
                    return success;

                auto params = command_action_msg->params();
                auto action_it = params.find("action");
                if (action_it != params.end()) {
                    auto command = boost::get<std::string>(action_it->second);
                    if (command == "quit")
                        context()->engine()->quit();
                    else if (command == "update_working_directory") {
                        _working_directory.value(fmt::format(
                                " | {}",
                                boost::filesystem::current_path().string()));
                    }
                }
            }
            return success;
        });
        _console.initialize();
        _console.focus(&_console);

        _layout_panel.font_family(family);
        _layout_panel.palette(context()->palette());
        _layout_panel.dock(core::dock::styles::fill);
        _layout_panel.fg_color(ide::colors::info_text);
        _layout_panel.bg_color(ide::colors::fill_color);
        _layout_panel.add_child(&_header);
        _layout_panel.add_child(&_footer);
        _layout_panel.add_child(&_console);
    }

    void controller::on_update(uint32_t dt) {
        _console.update(dt);
    }

    void controller::on_draw(core::renderer& surface) {
        _layout_panel.draw(surface);
    }

    void controller::on_resize(const core::rect& bounds) {
        _layout_panel.resize(bounds);
    }

    bool controller::on_process_event(const SDL_Event* e) {
        return _layout_panel.process_event(e);
    }

    void controller::on_activate(const core::parameter_dict& params) {
        if (_show_banner) {
            _console.write_message("<rev> <bold>Ryu: <italic>The Arcade Construction Kit <>");
            _console.write_message(" Copyright (C) 2017 Jeff Panici");
            _console.write_message(" See details in <underline><bold>LICENSE<> file");
            _console.caret_down();
            _console.write_message("Ready.");
            _show_banner = false;
        }
    }

}