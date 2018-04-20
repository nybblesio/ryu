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

    controller::controller(const std::string& name) : ryu::core::state(name) {
    }

    void controller::define_actions() {
        auto leave_action = core::input_action::create_no_map(
                "source_editor_leave",
                "Internal",
                "Close the source editor and return to previous state.");
        leave_action->bind_keys({core::key_escape});

        auto command_bar_action = core::input_action::create_no_map(
                "source_editor_command_bar",
                "Internal",
                "Activate the command bar.");
        command_bar_action->bind_keys({core::mod_ctrl, core::key_space});
    }

    void controller::bind_events() {
//        leave_action->register_handler(
//            core::action_sink::controller,
//            [this](const core::event_data_t& data) {
//                return is_focused();
//            },
//            [this](const core::event_data_t& data) {
//                end_state();
//                return true;
//            });
//
//        command_bar_action->register_handler(
//            core::action_sink::controller,
//            [this](const core::event_data_t& data) {
//                return is_focused();
//            },
//            [this](const core::event_data_t& data) {
//                _layout_panel->focus(_command_line.get());
//                return true;
//            });
    }

    void controller::on_deactivate() {
        _layout_panel->visible(false);
    }

    void controller::on_initialize() {
        bind_events();

        _header = core::view_factory::create_state_header(
            this,
            "header-panel",
            ide::colors::info_text,
            ide::colors::fill_color,
            core::dock::styles::top,
            {_metrics.left_padding, _metrics.right_padding, 5, 0});
        _header->state("source editor");
        _header->state_color(ide::colors::white);
        _header->custom("| file: (none)");

        _command_line = core::view_factory::create_textbox(
            this,
            "command-line-textbox",
            ide::colors::text,
            ide::colors::fill_color,
            "",
            core::dock::styles::top,
            {_metrics.left_padding, _metrics.right_padding * 3, 0, 10});
        _command_line->width(60);
        _command_line->length(255);
        _command_line->sizing(core::view::sizing::types::parent);
        _command_line->on_key_down([&](int key_code) {
            switch (key_code) {
                case core::ascii_escape: {
                    _layout_panel->focus(_editor.get());
                    return true;
                }
                case core::ascii_return: {
                    core::result result;
                    auto input = _command_line->value();
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
                            _editor->save(result);
                        } else if (command == "read_text") {
                            auto name = command_action_msg->get_parameter<std::string>("name");
                            if (!name.empty()) {
                                _editor->load(result, name);
                            } else {
                                // XXX: handle errors
                            }
                        } else if (command == "write_text") {
                            auto name = command_action_msg->get_parameter<std::string>("name");
                            if (!name.empty()) {
                                _editor->save(result, name);
                            } else {
                                // XXX: handle errors
                            }
                        } else if (command == "clear") {
                            _editor->clear();
                        } else if (command == "goto_line") {
                            auto line_number = command_action_msg->get_parameter<uint32_t>("line_number");
                            _editor->goto_line(line_number);
                        } else if (command == "find_text") {
                            auto needle = command_action_msg->get_parameter<std::string>("needle");
                            if (!needle.empty()) {
                                _editor->find(needle);
                            } else {
                                // XXX: handle errors
                            }
                        } else {
                            // XXX: unknown command, error!
                        }
                    }

                    _command_line->clear();
                    _layout_panel->focus(_editor.get());

                    return true;
                }
                default: {
                    return true;
                }
            }
        });

        _footer = core::view_factory::create_document_footer(
                this,
                "footer-panel",
                ide::colors::info_text,
                ide::colors::fill_color,
                core::dock::styles::bottom,
                {_metrics.left_padding, _metrics.right_padding, 5, 5});

        _editor = core::view_factory::create_text_editor(
            this,
            "text-editor",
            ide::colors::text,
            ide::colors::fill_color,
            rows,
            columns);
        _editor->caret_color(ide::colors::caret);
        _editor->selection_color(ide::colors::selection);
        _editor->line_number_color(ide::colors::info_text);
        _editor->on_caret_changed([&](const core::caret& caret, const core::document& document) {
            std::string file_name = document.path().filename().string();
            if (file_name.empty()) {
                file_name = "(none)";
            }
            _header->custom(fmt::format("| file: {}", file_name));
            _footer->value(caret, document);
        });

        _layout_panel = core::view_factory::create_dock_layout_panel(
            this,
            "layout-panel",
            ide::colors::info_text,
            ide::colors::fill_color);
        _layout_panel->add_child(_header.get());
        _layout_panel->add_child(_command_line.get());
        _layout_panel->add_child(_footer.get());
        _layout_panel->add_child(_editor.get());
        _layout_panel->focus(_editor.get());
    }

    void controller::on_update(uint32_t dt) {
    }

    void controller::on_draw(core::renderer& surface) {
        _layout_panel->draw(surface);
    }

    void controller::on_resize(const core::rect& bounds) {
        _layout_panel->resize(bounds);
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

            if (!_editor->load(result, path)) {
                _editor->clear();
                _editor->path(path);
            }
        }

        _layout_panel->visible(true);
    }

}