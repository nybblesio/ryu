//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <core/engine.h>
#include <core/project.h>
#include <ide/ide_context.h>
#include <core/project_file.h>
#include "controller.h"

namespace ryu::ide::hex_editor {

    controller::controller(const std::string& name) : core::state(name),
                                                      _caret_status("caret-status"),
                                                      _machine_status("machine-status"),
                                                      _project_status("project-status"),
                                                      _command_line("command-line"),
                                                      _editor("memory-editor"),
                                                      _environment_status("environment_status"),
                                                      _header("header-label"),
                                                      _footer("footer-label"),
                                                      _layout_panel("layout-panel") {
    }

    void controller::on_initialize() {
        _project_status.font_family(context()->font_family());
        _project_status.palette(&context()->palette());
        _project_status.dock(core::dock::styles::left);
        _project_status.fg_color(ide::colors::info_text);
        _project_status.bg_color(ide::colors::fill_color);
        _project_status.margin({0, context()->font_face()->width, 0, 0});
        _project_status.value("project: (none)");

        _machine_status.font_family(context()->font_family());
        _machine_status.palette(&context()->palette());
        _machine_status.dock(core::dock::styles::left);
        _machine_status.fg_color(ide::colors::info_text);
        _machine_status.bg_color(ide::colors::fill_color);
        _machine_status.margin({0, context()->font_face()->width, 0, 0});
        _machine_status.value("| machine: (none)");

        _header.font_family(context()->font_family());
        _header.palette(&context()->palette());
        _header.dock(core::dock::styles::top);
        _header.fg_color(ide::colors::info_text);
        _header.bg_color(ide::colors::fill_color);
        _header.bounds().height(context()->font_face()->line_height);
        _header.margin({_metrics.left_padding, _metrics.right_padding, 5, 0});
        _header.add_child(&_project_status);
        _header.add_child(&_machine_status);

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
            _project_status.value(fmt::format("project: {}", project_name));
            _machine_status.value(fmt::format(" | machine: {}", machine_name));
        });

        _caret_status.font_family(context()->font_family());
        _caret_status.margin({0, 0, 0, 0});
        _caret_status.palette(&context()->palette());
        _caret_status.dock(core::dock::styles::left);
        _caret_status.fg_color(ide::colors::info_text);
        _caret_status.bg_color(ide::colors::fill_color);

        _environment_status.font_family(context()->font_family());
        _environment_status.margin({0, 0, 0, 0});
        _environment_status.palette(&context()->palette());
        _environment_status.dock(core::dock::styles::left);
        _environment_status.fg_color(ide::colors::info_text);
        _environment_status.bg_color(ide::colors::fill_color);
        _environment_status.value(fmt::format(
                " | env: {}",
                context()->environment()->name()));

        _footer.font_family(context()->font_family());
        _footer.palette(&context()->palette());
        _footer.dock(core::dock::styles::bottom);
        _footer.bounds().height(context()->font_face()->line_height);
        _footer.fg_color(ide::colors::info_text);
        _footer.bg_color(ide::colors::fill_color);
        _footer.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
        _footer.add_child(&_caret_status);
        _footer.add_child(&_environment_status);

        _command_line.width(60);
        _command_line.length(255);
        _command_line.font_family(context()->font_family());
        _command_line.palette(&context()->palette());
        _command_line.dock(core::dock::styles::top);
        _command_line.fg_color(ide::colors::text);
        _command_line.bg_color(ide::colors::fill_color);
        _command_line.sizing(core::view::sizing::types::parent);
        _command_line.on_key_down([&](int keycode) {
            if (keycode == SDLK_ESCAPE) {
                _layout_panel.focus(&_editor);
                return true;
            }
            if (keycode == SDLK_RETURN) {
                core::result result;
                auto input = _command_line.value();
                auto success = context()->environment()->execute(result, input);
                if (success) {
                    auto command_action_msg = result.find_code("command_action");
                    if (command_action_msg == nullptr)
                        return success;

                    // XXX: need to refactor this, it makes my head hurt
                    auto command = command_action_msg->get_parameter<std::string>("action");
                    if (command == "quit") {
                        context()->engine()->quit();
                    } else if (command == "read_text") {
                        auto name = command_action_msg->get_parameter<std::string>("name");
                        if (!name.empty()) {
                            //_editor.load(result, boost::get<std::string>(name_it->second));
                        } else {
                            // XXX: handle errors
                        }
                    } else if (command == "write_text") {
                        auto name = command_action_msg->get_parameter<std::string>("name");
                        if (!name.empty()) {
                            //_editor.save(result, boost::get<std::string>(name_it->second));
                        } else {
                            // XXX: handle errors
                        }
                    } else if (command == "goto_line") {
                        auto line_number = command_action_msg->get_parameter<uint32_t>("line_number");
                        _editor.goto_address(line_number);
                    } else {
                        // XXX: unknown command, error!
                    }
                }

                _command_line.clear();
                _layout_panel.focus(&_editor);
                return true;
            }
            return true;
        });
        _command_line.margin({_metrics.left_padding, _metrics.right_padding * 3, 0, 10});

        _editor.font_family(context()->font_family());
        _editor.palette(&context()->palette());
        _editor.dock(core::dock::styles::fill);
        _editor.fg_color(ide::colors::text);
        _editor.caret_color(ide::colors::caret);
        _editor.selection_color(ide::colors::selection);
        _editor.address_color(ide::colors::info_text);
        _editor.on_caret_changed([&](const core::caret& caret) {
            _caret_status.value(fmt::format(
                    "| X:{:03d} Y:{:02d} | {}",
                    caret.column() + 1,
                    caret.row() + 1,
                    caret.mode() == core::caret::mode::overwrite ? "OVR" : "INS"));
        });
        _editor.initialize(65535, 16);

        _layout_panel.font_family(context()->font_family());
        _layout_panel.palette(&context()->palette());
        _layout_panel.dock(core::dock::styles::fill);
        _layout_panel.fg_color(ide::colors::info_text);
        _layout_panel.bg_color(ide::colors::fill_color);
        _layout_panel.add_child(&_header);
        _layout_panel.add_child(&_command_line);
        _layout_panel.add_child(&_footer);
        _layout_panel.add_child(&_editor);

        core::project::add_listener([&](){
            auto env = context()->environment();
            auto project = core::project::instance();
            if (project != nullptr) {
                auto active_environment = project->active_environment();
                if (active_environment != nullptr) {
                    env->name(active_environment->path().filename().string());
                }
            } else {
                env->name("");
            }
            _environment_status.value(fmt::format(" | env: {}", env->name()));
        });
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
        auto ctrl_pressed = (SDL_GetModState() & KMOD_CTRL) != 0;

        if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_ESCAPE: {
                    end_state();
                    return true;
                }
                case SDLK_SPACE: {
                    if (ctrl_pressed) {
                        _layout_panel.focus(&_command_line);
                        return true;
                    }
                    break;
                }
                default:
                    break;
            }
        }

        return _layout_panel.process_event(e);
    }

    void controller::on_activate(const core::parameter_dict& params) {
    }

}