//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include <core/engine.h>
#include <ide/ide_types.h>
#include <ide/environment.h>
#include "controller.h"

namespace ryu::ide::text_editor {

    controller::controller(const std::string& name) : core::state(name),
                                                      _editor("text-editor"),
                                                      _cpu_status("cpu-status-label"),
                                                      _file_status("file-status-label"),
                                                      _caret_status("caret-status-label"),
                                                      _command_line("command-line"),
                                                      _document_status("document-status-label"),
                                                      _footer("footer-panel"),
                                                      _header("header-panel"),
                                                      _layout_panel("layout-panel") {
    }

    void controller::on_initialize() {
        auto family = context()->engine()->find_font_family("hack");
        auto face = family->find_style(core::font::styles::normal);

        _cpu_status.font_family(family);
        _cpu_status.palette(context()->palette());
        _cpu_status.dock(core::dock::styles::left);
        _cpu_status.fg_color(ide::colors::info_text);
        _cpu_status.bg_color(ide::colors::fill_color);
        _cpu_status.margin({0, face->width, 0, 0});
        _cpu_status.value("cpu: (none)");

        _file_status.font_family(family);
        _file_status.margin({0, 0, 0, 0});
        _file_status.value("| file: (none)");
        _file_status.palette(context()->palette());
        _file_status.dock(core::dock::styles::left);
        _file_status.fg_color(ide::colors::info_text);
        _file_status.bg_color(ide::colors::fill_color);
        _file_status.sizing(core::view::sizing::types::fixed);
        _file_status.bounds().size(face->measure_chars(46), face->line_height);

        _header.font_family(family);
        _header.palette(context()->palette());
        _header.dock(core::dock::styles::top);
        _header.fg_color(ide::colors::info_text);
        _header.bg_color(ide::colors::fill_color);
        _header.bounds().height(face->line_height);
        _header.margin({_metrics.left_padding, _metrics.right_padding, 5, 0});
        _header.add_child(&_cpu_status);
        _header.add_child(&_file_status);

        _command_line.width(60);
        _command_line.length(255);
        _command_line.font_family(family);
        _command_line.palette(context()->palette());
        _command_line.dock(core::dock::styles::top);
        _command_line.fg_color(ide::colors::text);
        _command_line.bg_color(ide::colors::fill_color);
        _command_line.sizing(core::view::sizing::types::parent);
        _command_line.on_key_down([&](int keycode) {
            if (keycode == SDLK_ESCAPE) {
                _layout_panel.focus(_editor.id());
                return false;
            }
            if (keycode == SDLK_RETURN) {
                auto input = _command_line.value();
                core::result result;
                ide::environment::instance()->execute(result, input);
                if (result.has_code("C001"))
                    context()->engine()->quit();
                else if (result.has_code("C004")) {
                    _editor.clear();
                } else if (result.has_code("C020")) {
                    uint32_t line = static_cast<uint32_t>(std::atoi(result.messages()[0].message().c_str()));
                    _editor.goto_line(line);
                } else if (result.has_code("C021")) {
                    _editor.load(result.messages()[0].message());
                } else if (result.has_code("C022")) {
                    auto path = result.messages()[0].message();
                    _editor.save(path == "(default)" ? "" : path);
                }
                _command_line.clear();
                _layout_panel.focus(_editor.id());
                return false;
            }
            return true;
        });
        _command_line.margin({_metrics.left_padding, _metrics.right_padding * 3, 0, 10});

        _document_status.font_family(family);
        _document_status.palette(context()->palette());
        _document_status.dock(core::dock::styles::left);
        _document_status.fg_color(ide::colors::info_text);
        _document_status.bg_color(ide::colors::fill_color);
        _document_status.margin({0, face->width, 0, 0});

        _caret_status.font_family(family);
        _caret_status.palette(context()->palette());
        _caret_status.dock(core::dock::styles::left);
        _caret_status.fg_color(ide::colors::info_text);
        _caret_status.bg_color(ide::colors::fill_color);
        _caret_status.margin({0, 0, 0, 0});

        _footer.font_family(family);
        _footer.palette(context()->palette());
        _footer.dock(core::dock::styles::bottom);
        _footer.bounds().height(face->line_height);
        _footer.fg_color(ide::colors::info_text);
        _footer.bg_color(ide::colors::fill_color);
        _footer.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
        _footer.add_child(&_document_status);
        _footer.add_child(&_caret_status);

        _editor.font_family(family);
        _editor.dock(core::dock::styles::fill);
        _editor.palette(context()->palette());
        _editor.on_document_changed([&](const core::document& document) {
            std::string file_name = document.filename();
            if (file_name.empty()) {
                file_name = "(none)";
            }
            _file_status.value(fmt::format("| file: {0}", file_name));
            _document_status.value(fmt::format(
                    "C:{0:03d}/{1:03d} R:{2:04d}/{3:04d}",
                    document.column() + 1,
                    document.columns(),
                    document.row() + 1,
                    document.rows()));
        });
        _editor.on_caret_changed([&](const core::caret& caret) {
            _caret_status.value(fmt::format(
                    "| X:{0:03d} Y:{1:02d} | {2}",
                    caret.column() + 1,
                    caret.row() + 1,
                    caret.mode() == core::caret::mode::overwrite ? "OVR" : "INS"));
        });
        _editor.initialize(rows, columns);

        _layout_panel.font_family(family);
        _layout_panel.palette(context()->palette());
        _layout_panel.dock(core::dock::styles::fill);
        _layout_panel.fg_color(ide::colors::info_text);
        _layout_panel.bg_color(ide::colors::fill_color);
        _layout_panel.add_child(&_header);
        _layout_panel.add_child(&_command_line);
        _layout_panel.add_child(&_footer);
        _layout_panel.add_child(&_editor);

        _layout_panel.focus(_editor.id());
    }

    core::project* controller::project() {
        return _project;
    }

    void controller::on_update(uint32_t dt) {
    }

    hardware::machine* controller::machine() {
        return _machine;
    }

    void controller::project(core::project* value) {
        _project = value;
    }

    void controller::machine(hardware::machine* value) {
        _machine = value;
    }

    void controller::on_draw(core::renderer& surface) {
        _layout_panel.draw(surface);
    }

    void controller::on_resize(const core::rect& bounds) {
        _layout_panel.resize(bounds);
    }

    bool controller::on_process_event(const SDL_Event* e) {
        auto ctrl_pressed = (SDL_GetModState() & KMOD_CTRL) != 0;

        if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_ESCAPE: {
                    if (!ctrl_pressed) {
                        end_state();
                        return true;
                    } else {
                        _layout_panel.focus(_command_line.id());
                        return true;
                    }
                }
            }
        }
        return _layout_panel.process_event(e);
    }

}