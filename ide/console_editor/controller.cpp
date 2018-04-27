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

    static logger* s_log = logger_factory::instance()->create(
        "console::controller",
        logger::level::info);

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
            "assets/views/console.yaml");
        s_log->result(result);

        _console = _layout_panel->find_by_name<core::console>("console-view");
        _footer = _layout_panel->find_by_name<core::document_footer>("footer-panel");

        _console->code_mapper(s_mapper);
        _console->on_transition([&](core::state_transition_command command, const core::parameter_dict& params) {
            return transition_to(command, params);
        });
        _console->on_caret_changed([&](const core::caret& caret, const core::document& document) {
            _footer->update_state(caret, document);
        });
        _console->on_execute_command([&](core::result& result, const std::string& input) {
            auto success = context()->environment()->execute(result, input);
            if (success) {
                auto command_action_msg = result.find_code("command_action");
                if (command_action_msg == nullptr)
                    return success;

                auto command = command_action_msg->get_parameter<core::system_commands::types>("action");
                switch (command) {
                    case core::system_commands::quit:
                        context()->engine()->quit();
                        break;
                    default:
                        // XXX: handle this error case
                        break;
                }
            }
            return success;
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
        if (!_show_banner)
            return;

        _console->write_message("<rev> <bold>Ryu: <italic>The Arcade Construction Kit <>");
        _console->write_message(" Copyright (C) 2017 Jeff Panici");
        _console->write_message(" See details in <underline><bold>LICENSE<> file");
        _console->caret_down();
        _console->write_message("Ready.");
        _show_banner = false;
    }

    void controller::on_update(uint32_t dt, core::pending_event_list& events) {
        _layout_panel->update(dt, events);
    }

}