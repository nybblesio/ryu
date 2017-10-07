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
#include <ide/environment.h>
#include "controller.h"

namespace ryu::ide::console {

    controller::controller(const std::string& name) : core::state(name),
                                                      _header("header-label"),
                                                      _footer("footer-label"),
                                                      _console("console"),
                                                      _layout_panel("layout-panel") {
    }

    void controller::on_initialize() {
        auto family = context()->engine()->find_font_family("hack");

        _header.palette(context()->palette());
        _header.dock(core::dock::styles::top);
        _header.font_family(family);
        _header.fg_color(ide::colors::info_text);
        _header.bg_color(ide::colors::fill_color);
        _header.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});

        // TODO: this is temporarily located here, it needs to be in its own method
        std::string project_name = "(none)";
        std::string machine_name = "(none)";
//        auto project = dynamic_cast<ide::ide_context*>(context())->project();
//        if (project != nullptr) {
//            project_name = project->name();
//            machine_name = project->machine()->name();
//        }
        _header.value(fmt::format("project: {0} | machine: {1}", project_name, machine_name));
        // TODO: ^^^^ temporary

        _footer.palette(context()->palette());
        _footer.dock(core::dock::styles::bottom);
        _footer.font_family(family);
        _footer.fg_color(ide::colors::info_text);
        _footer.bg_color(ide::colors::fill_color);
        _footer.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});

        _console.font_family(family);
        _console.palette(context()->palette());
        _console.dock(core::dock::styles::fill);
        _console.on_transition([&](const std::string& name, const core::parameter_dict& params) {
            return transition_to(name, params);
        });
        _console.on_caret_changed([&](const core::caret& caret) {
            _footer.value(fmt::format(
                    "X:{0:03d} Y:{1:02d} | {2}",
                    caret.column() + 1,
                    caret.row() + 1,
                    caret.mode() == core::caret::mode::overwrite ? "OVR" : "INS"));
        });
        _console.on_execute_command([&](core::result& result, const std::string& input) {
            auto success = ide::environment::instance()->execute(result, input);
            if (success && result.has_code("C001")) {
                context()->engine()->quit();
            }
            return success;
        });
        _console.initialize();
        _console.focus(_console.id());

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
            _console.write_message("{rev} {bold}Ryu: {italic}The Arcade Construction Kit {}");
            _console.write_message(" Copyright (C) 2017 Jeff Panici");
            _console.write_message(" See details in {underline}{bold}LICENSE{} file");
            _console.caret_down();
            _console.write_message("Ready.");
            _show_banner = false;
        }
    }

}