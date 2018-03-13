//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include <core/engine.h>
#include <ide/ide_types.h>
#include <core/environment.h>
#include <core/project_file.h>
#include <core/input_action.h>
#include "controller.h"

namespace ryu::ide::source_editor {

    controller::controller(const std::string& name) : ryu::core::state(name),
                                                      _file_status("file-status-label", this),
                                                      _caret_status("caret-status-label", this),
                                                      _editor("text-editor", this),
                                                      _project_label("project-label", this),
                                                      _machine_label("machine-label", this),
                                                      _command_line("command-line", this),
                                                      _document_status("document-status-label", this),
                                                      _footer("footer-panel", this),
                                                      _header("header-panel", this),
                                                      _layout_panel("layout-panel", this) {
    }

    void controller::bind_events() {
        auto leave_action = core::input_action::create_no_map(
            "source_editor_leave",
            "Internal",
            "Close the source editor and return to previous state.");
        leave_action->register_handler(
            core::action_sink::controller,
            core::action_sink::default_filter,
            [this](const core::event_data_t& data) {
                end_state();
                return true;
            });
        leave_action->bind_keys({core::key_escape});

        auto command_bar_action = core::input_action::create_no_map(
            "source_editor_command_bar",
            "Internal",
            "Activate the command bar.");
        command_bar_action->register_handler(
            core::action_sink::controller,
            core::action_sink::default_filter,
            [this](const core::event_data_t& data) {
                _layout_panel.focus(&_command_line);
                return true;
            });
        command_bar_action->bind_keys({core::mod_ctrl, core::key_space});
    }

    void controller::on_initialize() {
        bind_events();

        _project_label.font_family(context()->font_family());
        _project_label.palette(&context()->palette());
        _project_label.dock(core::dock::styles::left);
        _project_label.fg_color(ide::colors::info_text);
        _project_label.bg_color(ide::colors::fill_color);
        _project_label.margin({0, context()->font_face()->width, 0, 0});
        _project_label.value("project: (none)");

        _machine_label.font_family(context()->font_family());
        _machine_label.palette(&context()->palette());
        _machine_label.dock(core::dock::styles::left);
        _machine_label.fg_color(ide::colors::info_text);
        _machine_label.bg_color(ide::colors::fill_color);
        _machine_label.margin({0, context()->font_face()->width, 0, 0});
        _machine_label.value("| machine: (none)");

        _file_status.font_family(context()->font_family());
        _file_status.margin({0, 0, 0, 0});
        _file_status.value("| file: (none)");
        _file_status.palette(&context()->palette());
        _file_status.dock(core::dock::styles::left);
        _file_status.fg_color(ide::colors::info_text);
        _file_status.bg_color(ide::colors::fill_color);

        core::project::add_listener([&]() {
            std::string project_name = "(none)";
            std::string machine_name = "(none)";
            std::string file = "(none)";

            if (core::project::instance() != nullptr) {
                project_name = core::project::instance()->name();
                if (core::project::instance()->dirty())
                    project_name += "*";
                if (core::project::instance()->machine() != nullptr) {
                    machine_name = core::project::instance()->machine()->name();
                }
            }

            _project_label.value(fmt::format("project: {}", project_name));
            _machine_label.value(fmt::format("| machine: {}", machine_name));
            _file_status.value(fmt::format("| file: {}", file));
        });

        _header.font_family(context()->font_family());
        _header.palette(&context()->palette());
        _header.dock(core::dock::styles::top);
        _header.fg_color(ide::colors::info_text);
        _header.bg_color(ide::colors::fill_color);
        _header.bounds().height(context()->font_face()->line_height);
        _header.margin({_metrics.left_padding, _metrics.right_padding, 5, 0});
        _header.add_child(&_project_label);
        _header.add_child(&_machine_label);
        _header.add_child(&_file_status);

        _command_line.width(60);
        _command_line.length(255);
        _command_line.font_family(context()->font_family());
        _command_line.palette(&context()->palette());
        _command_line.dock(core::dock::styles::top);
        _command_line.fg_color(ide::colors::text);
        _command_line.bg_color(ide::colors::fill_color);
        _command_line.sizing(core::view::sizing::types::parent);
        _command_line.on_key_down([&](int keycode) {
            if (keycode == 27) {
                _layout_panel.focus(&_editor);
                return true;
            }
            if (keycode == 13) {
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
                    } else if (command == "save_project_file") {
                        _editor.save(result);
                    } else if (command == "read_text") {
                        auto name = command_action_msg->get_parameter<std::string>("name");
                        if (!name.empty()) {
                            _editor.load(result, name);
                        } else {
                            // XXX: handle errors
                        }
                    } else if (command == "write_text") {
                        auto name = command_action_msg->get_parameter<std::string>("name");
                        if (!name.empty()) {
                            _editor.save(result, name);
                        } else {
                            // XXX: handle errors
                        }
                    } else if (command == "clear") {
                        _editor.clear();
                    } else if (command == "goto_line") {
                        auto line_number = command_action_msg->get_parameter<uint32_t>("line_number");
                        _editor.goto_line(line_number);
                    } else if (command == "find_text") {
                        auto needle = command_action_msg->get_parameter<std::string>("needle");
                        if (!needle.empty()) {
                            _editor.find(needle);
                        } else {
                            // XXX: handle errors
                        }
                    }
                    else {
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

        _document_status.font_family(context()->font_family());
        _document_status.palette(&context()->palette());
        _document_status.dock(core::dock::styles::left);
        _document_status.fg_color(ide::colors::info_text);
        _document_status.bg_color(ide::colors::fill_color);
        _document_status.margin({0, context()->font_face()->width, 0, 0});

        _caret_status.font_family(context()->font_family());
        _caret_status.margin({0, 0, 0, 0});
        _caret_status.palette(&context()->palette());
        _caret_status.dock(core::dock::styles::left);
        _caret_status.fg_color(ide::colors::info_text);
        _caret_status.bg_color(ide::colors::fill_color);

        _footer.font_family(context()->font_family());
        _footer.palette(&context()->palette());
        _footer.dock(core::dock::styles::bottom);
        _footer.bounds().height(context()->font_face()->line_height);
        _footer.fg_color(ide::colors::info_text);
        _footer.bg_color(ide::colors::fill_color);
        _footer.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
        _footer.add_child(&_document_status);
        _footer.add_child(&_caret_status);

        _editor.font_family(context()->font_family());
        _editor.palette(&context()->palette());
        _editor.dock(core::dock::styles::fill);
        _editor.fg_color(ide::colors::text);
        _editor.caret_color(ide::colors::caret);
        _editor.selection_color(ide::colors::selection);
        _editor.line_number_color(ide::colors::info_text);
        _editor.on_caret_changed([&](const core::caret& caret, const core::document& document) {
            std::string file_name = document.path().filename().string();
            if (file_name.empty()) {
                file_name = "(none)";
            }
            _file_status.value(fmt::format("| file: {}", file_name));
            _document_status.value(fmt::format(
                    "C:{:03d}/{:03d} R:{:04d}/{:04d}",
                    document.column() + caret.column() + 1,
                    document.columns(),
                    document.row() + caret.row() + 1,
                    document.rows()));
            _caret_status.value(fmt::format(
                    "| X:{:03d} Y:{:02d} | {}",
                    caret.column() + 1,
                    caret.row() + 1,
                    caret.mode() == core::caret::mode::overwrite ? "OVR" : "INS"));
        });
        _editor.initialize(rows, columns);

        _layout_panel.font_family(context()->font_family());
        _layout_panel.palette(&context()->palette());
        _layout_panel.dock(core::dock::styles::fill);
        _layout_panel.fg_color(ide::colors::info_text);
        _layout_panel.bg_color(ide::colors::fill_color);
        _layout_panel.add_child(&_header);
        _layout_panel.add_child(&_command_line);
        _layout_panel.add_child(&_footer);
        _layout_panel.add_child(&_editor);

        _layout_panel.focus(&_editor);
    }

    void controller::on_update(uint32_t dt) {
    }

    void controller::on_draw(core::renderer& surface) {
        _layout_panel.draw(surface);
    }

    void controller::on_resize(const core::rect& bounds) {
        _layout_panel.resize(bounds);
    }

    void controller::on_activate(const core::parameter_dict& params) {
        core::result result;

        auto path_it = params.find("name");
        if (path_it != params.end()) {
            std::string path = boost::get<std::string>(path_it->second);

            if (core::project::instance() != nullptr) {
                auto file = core::project::instance()->find_file(path);
                if (file != nullptr)
                    path = file->full_path().string();
            }

            if (!_editor.load(result, path)) {
                _editor.clear();
                _editor.path(path);
            }
        }

        // XXX: handle result if errored
    }

}