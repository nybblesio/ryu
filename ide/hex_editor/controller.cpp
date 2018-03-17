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
#include <core/input_action.h>
#include <core/project_file.h>
#include "controller.h"

namespace ryu::ide::hex_editor {

    controller::controller(const std::string& name) : core::state(name) {
    }

    void controller::bind_events() {
        auto leave_action = core::input_action::create_no_map(
            "memory_editor_leave",
            "Internal",
            "Close the memory editor and return to previous state.");
        leave_action->register_handler(
            core::action_sink::controller,
            [this](const core::event_data_t& data) {
                return is_focused();
            },
            [this](const core::event_data_t& data) {
                end_state();
                return true;
            });
        leave_action->bind_keys({core::key_escape});

        auto command_bar_action = core::input_action::create_no_map(
            "memory_editor_command_bar",
            "Internal",
            "Activate the command bar.");
        command_bar_action->register_handler(
            core::action_sink::controller,
            [this](const core::event_data_t& data) {
                return is_focused();
            },
            [this](const core::event_data_t& data) {
                _layout_panel->focus(_command_line.get());
                return true;
            });
        command_bar_action->bind_keys({core::mod_ctrl, core::key_space});
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
        _header->state("memory editor");
        _header->state_color(ide::colors::white);

        _caret_status = core::view_factory::create_label(
            this,
            "caret-status-label",
            ide::colors::info_text,
            ide::colors::fill_color);

        _footer = core::view_factory::create_dock_layout_panel(
            this,
            "footer-panel",
            ide::colors::info_text,
            ide::colors::fill_color,
            core::dock::styles::bottom,
            {_metrics.left_padding, _metrics.right_padding, 5, 5});
        _footer->bounds().height(context()->font_face()->line_height);
        _footer->add_child(_caret_status.get());

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
            if (key_code == core::ascii_escape) {
                _layout_panel->focus(_editor.get());
                return true;
            }
            if (key_code == core::ascii_return) {
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
                        _editor->goto_address(line_number);
                    } else {
                        // XXX: unknown command, error!
                    }
                }

                _command_line->clear();
                _layout_panel->focus(_editor.get());
                return true;
            }
            return true;
        });

        _editor = core::view_factory::create_memory_editor(
            this,
            "memory-editor",
            ide::colors::text,
            ide::colors::fill_color);
        _editor->caret_color(ide::colors::caret);
        _editor->selection_color(ide::colors::selection);
        _editor->address_color(ide::colors::info_text);
        _editor->on_caret_changed([&](const core::caret& caret) {
            _caret_status->value(fmt::format(
                    "| X:{:03d} Y:{:02d} | {}",
                    caret.column() + 1,
                    caret.row() + 1,
                    caret.mode() == core::caret::mode::overwrite ? "OVR" : "INS"));
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
    }

}