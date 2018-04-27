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

    static logger* s_log = logger_factory::instance()->create(
        "source_editor::controller",
        logger::level::info);

    controller::controller(const std::string& name) : ryu::core::state(name) {
    }

    void controller::define_actions() {
        auto leave_action = core::input_action::create_no_map(
                "source_editor_leave",
                "Internal",
                "Close the source editor and return to previous state.");
        if (!leave_action->has_bindings())
            leave_action->bind_keys({core::key_escape});

        auto command_bar_action = core::input_action::create_no_map(
                "source_editor_command_bar",
                "Internal",
                "Activate the command bar.");
        if (!command_bar_action->has_bindings())
            command_bar_action->bind_keys({core::mod_ctrl, core::key_space});
    }

    void controller::bind_events() {
        action_provider().register_handler(
                core::input_action::find_by_name("source_editor_leave"),
                [this](const core::event_data_t& data) {
                    end_state();
                    return true;
                });

        action_provider().register_handler(
                core::input_action::find_by_name("source_editor_command_bar"),
                [this](const core::event_data_t& data) {
                    _layout_panel->focus(_command_line);
                    return true;
                });
    }

    void controller::on_deactivate() {
        _layout_panel->visible(false);
    }

    void controller::on_initialize() {
        define_actions();
        bind_events();
    }

    bool controller::on_load(core::result& result) {
        _layout_panel = core::view_factory::create_loadable_view(
            this,
            "loadable-view",
            context()->font_family(),
            &context()->palette(),
            context()->prefs(),
            ide::colors::info_text,
            ide::colors::fill_color,
            result,
            "assets/views/source-editor.yaml");
        s_log->result(result);

        _editor = _layout_panel->find_by_name<core::text_editor>("text-editor");
        _header = _layout_panel->find_by_name<core::state_header>("header-panel");
        _footer = _layout_panel->find_by_name<core::document_footer>("footer-panel");
        _command_line = _layout_panel->find_by_name<core::text_box>("command-line-text-box");

        _header->custom("| file: (none)");

        _command_line->on_key_down([&](int key_code) {
            switch (key_code) {
                case core::ascii_escape: {
                    _layout_panel->focus(_editor);
                    return true;
                }
                case core::ascii_return: {
                    core::result command_result;
                    auto input = _command_line->value();
                    auto success = context()->environment()->execute(command_result, input);
                    if (success) {
                        auto command_action_msg = command_result.find_code("command_action");
                        if (command_action_msg == nullptr)
                            return success;

                        // XXX: need to refactor this, it makes my head hurt
                        auto command = command_action_msg->get_parameter<std::string>("action");
                        if (command == "quit") {
                            context()->engine()->quit();
                        } else if (command == "save_project_file") {
                            _editor->save(command_result);
                        } else if (command == "read_text") {
                            auto name = command_action_msg->get_parameter<std::string>("name");
                            if (!name.empty()) {
                                _editor->load(command_result, name);
                            } else {
                                // XXX: handle errors
                            }
                        } else if (command == "write_text") {
                            auto name = command_action_msg->get_parameter<std::string>("name");
                            if (!name.empty()) {
                                _editor->save(command_result, name);
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
                    _layout_panel->focus(_editor);

                    return true;
                }
                default: {
                    return true;
                }
            }
        });

        _editor->on_caret_changed([&](const core::caret& caret, const core::document& document) {
            std::string file_name = document.path().filename().string();
            if (file_name.empty()) {
                file_name = "(none)";
            }
            _header->custom(fmt::format("| file: {}", file_name));
            _footer->update_state(caret, document);
        });

        return !result.is_failed();
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

    void controller::on_update(uint32_t dt, core::pending_event_list& events) {
        _layout_panel->update(dt, events);
    }

}