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
#include <fmt/format.h>
#include <core/engine.h>
#include <ide/ide_context.h>
#include <common/bytes.h>
#include "console_view.h"

namespace ryu::ide::console {

    console_view::console_view(const std::string& name) : core::view(core::view::types::container, name),
                                                          _caret("console-caret"),
                                                          _header("header-label"),
                                                          _footer("footer-label") {
    }

    console_view::~console_view() {
    }

    void console_view::caret_end() {
        _caret.column(_metrics.page_width);
    }

    void console_view::caret_home() {
        _caret.column(0);
    }

    void console_view::caret_up(int rows) {
        _caret.up(rows);
    }

    void console_view::caret_down(int rows) {
        if (_caret.down(rows))
            _document.shift_up();
    }

    bool console_view::caret_left(int columns) {
        if (_caret.left(columns)) {
            caret_up();
            caret_end();
            return true;
        }
        return false;
    }

    bool console_view::caret_right(int columns) {
        if (_caret.right(columns)) {
            caret_down();
            caret_home();
            return true;
        }
        return false;
    }

    void console_view::initialize() {
        _color = ryu::ide::ide_context::colors::text;

        _header.palette(palette());
        _header.dock(dock::styles::top);
        _header.font_family(font_family());
        _header.fg_color(ide::ide_context::colors::info_text);
        _header.bg_color(ide::ide_context::colors::fill_color);
        _header.bounds().height(font_face()->line_height);
        _header.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});

        _footer.palette(palette());
        _footer.dock(dock::styles::bottom);
        _footer.font_family(font_family());
        _footer.fg_color(ide::ide_context::colors::info_text);
        _footer.bg_color(ide::ide_context::colors::fill_color);
        _footer.bounds().height(font_face()->line_height);
        _footer.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});

        _caret.overwrite();
        _caret.initialize(0, 0);
        _caret.palette(palette());
        _caret.font_family(font_family());
        _caret.fg_color(ide::ide_context::colors::caret);

        add_child(&_header);
        add_child(&_footer);
        add_child(&_caret);

        dock(dock::styles::fill);
        margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
    }

    void console_view::calculate_page_metrics() {
        auto rect = bounds();
        if (rect.empty())
            return;
        _metrics.page_width = static_cast<short>(rect.width() / font_face()->width);
        _metrics.page_height = static_cast<short>(rect.height() / font_face()->line_height);
    }

    void console_view::on_resize(const core::rect& context_bounds) {
        core::view::on_resize(context_bounds);

        calculate_page_metrics();
        _caret.page_size(_metrics.page_height, _metrics.page_width);

        if (!_document.initialized()) {
            _document.initialize(_metrics.page_height, _metrics.page_width);
            _document.default_attr(core::attr_t {_color, core::font::styles::normal, core::font::flags::none});
            _document.clear();
        }
        _document.page_size(_metrics.page_height, _metrics.page_width);
    }

    void console_view::on_draw(SDL_Renderer* renderer) {
        auto bounds = client_bounds();
        auto pal = *palette();

        std::string project_name = "(none)";
        std::string machine_name = "(none)";
//        auto project = dynamic_cast<ide::ide_context*>(context())->project();
//        if (project != nullptr) {
//            project_name = project->name();
//            machine_name = project->machine()->name();
//        }
        _header.value(fmt::format("project: {0} | machine: {1}", project_name, machine_name));
        _footer.value(fmt::format(
                "X:{0:03d} Y:{1:02d} | {2}",
                _caret.column() + 1,
                _caret.row() + 1,
                _caret.mode() == core::caret::mode::overwrite ? "OVR" : "INS"));

        // XXX: this may not be correct because font metrics may change
        auto face = font_face();
        auto y = bounds.top();
        for (auto row = 0; row < _metrics.page_height; row++) {
            auto x = bounds.left();
            auto chunks = _document.get_line_chunks(row, 0, _metrics.page_width);
            for (const auto& chunk : chunks) {
                auto width = static_cast<int32_t>(face->width * chunk.text.length());
                auto color = pal[chunk.attr.color];

                if ((chunk.attr.flags & core::font::flags::reverse) != 0) {
                    set_color(renderer, color);
                    color = pal[ide::ide_context::colors::fill_color];
                    fill_rect(renderer, core::rect{x, y, width, face->line_height});
                }

                font_style(chunk.attr.style);
                draw_text(renderer, x, y, chunk.text, color);
                x += width;
            }
            y += face->line_height;
        }
    }

    bool console_view::on_process_event(const SDL_Event* e) {
        auto ctrl_pressed = (SDL_GetModState() & KMOD_CTRL) != 0;
        auto shift_pressed = (SDL_GetModState() & KMOD_SHIFT) != 0;
        auto mode = _caret.mode();

        if (e->type == SDL_TEXTINPUT) {
            if (mode == core::caret::mode::insert) {
                _document.shift_right(_caret.row(), _caret.column());
            }
            const char* c = &e->text.text[0];
            while (*c != '\0') {
                _document.put(
                        _caret.row(),
                        _caret.column(),
                        core::element_t {static_cast<uint8_t>(*c), core::attr_t{_color}});
                caret_right();
                c++;
            }
        } else if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_ESCAPE: {
                    return transition_to("text_editor", {});
                }
                case SDLK_RETURN: {
                    core::result result;
                    std::stringstream cmd;

                    if (!shift_pressed) {
                        // first, seek backwards to a null value
                        while (true) {
                            caret_left();
                            if (_caret.row() == 0 && _caret.column() == 0)
                                break;
                            auto element = _document.get(_caret.row(), _caret.column());
                            if (element == nullptr || element->value == 0)
                                break;
                        }

                        // second, scan forward to the next null value to build the command
                        //caret_right();
                        while (true) {
                            caret_right();
                            auto element = _document.get(_caret.row(), _caret.column());
                            if (element == nullptr || element->value == 0)
                                break;
                            cmd << element->value;
                        }

                        // finally, if we got something, try to execute it
                        auto str = cmd.str();
                        if (str.length() > 0 && _execute_command_callback != nullptr) {
                            auto success = _execute_command_callback(result, str);

                            caret_down();
                            caret_home();

                            for (const auto& msg : result.messages()) {
                                if (msg.type() == core::result_message::types::data)
                                    continue;
                                auto error_part = msg.is_error() ? "{bold}{red}ERROR:{} " : "";
                                write_message(fmt::format("{0}{1}", error_part, msg.message()));
                            }

                            caret_down();
                            caret_home();

                            auto consumed = true;

                            if (result.has_code("C004")) {
                                _document.clear();
                                caret_home();
                                _caret.row(0);
                            } else if (success) {
                                auto code = result.find_code("C023");
                                if (code != nullptr) {
                                    consumed = transition_to("machine_editor", code->params());
                                }

                                code = result.find_code("C002");
                                if (code != nullptr) {
                                    consumed = transition_to("text_editor", code->params());
                                }

                                code = result.find_code("C024");
                                if (code != nullptr) {
                                    consumed = transition_to("hex_editor", code->params());
                                }

                                code = result.find_code("C030");
                                if (code != nullptr) {
                                    const auto& params = code->params();
                                    auto it = params.find("type");
                                    if (it != params.end()) {
                                        auto type = it->second;
                                        if (type == "MACH") {
                                            consumed = transition_to("machine_editor", params);
                                        } else if (type == "TEXT") {
                                            consumed = transition_to("text_editor", params);
                                        } else if (type == "DATA") {
                                            consumed = transition_to("hex_editor", params);
                                        }
                                    }
                                }
                            }

                            write_message("Ready.");
                            return consumed;
                        }
                    }

                    caret_down();
                    caret_home();

                    return true;
                }
                case SDLK_DELETE:
                    _document.shift_left(_caret.row(), _caret.column());
                    return true;

                case SDLK_BACKSPACE: {
                    if (caret_left())
                        caret_left();
                    _document.shift_left(_caret.row(), _caret.column());
                    return true;
                }
                case SDLK_UP:
                    caret_up();
                    return true;

                case SDLK_DOWN:
                    caret_down();
                    return true;

                case SDLK_LEFT:
                    caret_left();
                    return true;

                case SDLK_RIGHT:
                    caret_right();
                    return true;

                case SDLK_HOME:
                    if (ctrl_pressed) {
                        _caret.row(0);
                    }
                    caret_home();
                    return true;

                case SDLK_END:
                    if (ctrl_pressed) {
                        _caret.row(_metrics.page_height - 1);
                    }
                    caret_end();
                    return true;

                case SDLK_INSERT:
                    if (mode == core::caret::mode::insert)
                        _caret.overwrite();
                    else
                        _caret.insert();
                    return true;
            }
        }
        return false;
    }

    void console_view::write_message(const std::string& message) {
        core::attr_t attr {_color, core::font::styles::normal, core::font::flags::none};

        caret_home();
        auto token = message.begin();
        while (token != message.end()) {
            if (*token == '\n') {
                caret_down();
                caret_home();
                ++token;
                continue;
            }
            if (*token == '{') {
                std::string code;
                while (*(++token) != '}') {
                    code += *token;
                }
                if (code == "bold") {
                    attr.style |= core::font::styles::bold;
                } else if (code == "italic") {
                    attr.style |= core::font::styles::italic;
                } else if (code == "underline") {
                    attr.style |= core::font::styles::underline;
                } else if (code == "rev") {
                    attr.flags |= core::font::flags::reverse;
                } else if (code == "black") {
                    attr.color = ide::ide_context::colors::black;
                } else if (code == "white") {
                    attr.color = ide::ide_context::colors::white;
                } else if (code == "red") {
                    attr.color = ide::ide_context::colors::red;
                } else if (code == "cyan") {
                    attr.color = ide::ide_context::colors::cyan;
                } else if (code == "purple") {
                    attr.color = ide::ide_context::colors::purple;
                } else if (code == "green") {
                    attr.color = ide::ide_context::colors::green;
                } else if (code == "blue") {
                    attr.color = ide::ide_context::colors::blue;
                } else if (code == "yellow") {
                    attr.color = ide::ide_context::colors::yellow;
                } else if (code == "orange") {
                    attr.color = ide::ide_context::colors::orange;
                } else if (code == "brown") {
                    attr.color = ide::ide_context::colors::brown;
                } else if (code == "pink") {
                    attr.color = ide::ide_context::colors::pink;
                } else if (code == "dgrey") {
                    attr.color = ide::ide_context::colors::dark_grey;
                } else if (code == "grey") {
                    attr.color = ide::ide_context::colors::grey;
                } else if (code == "lgreen") {
                    attr.color = ide::ide_context::colors::light_green;
                } else if (code == "lblue") {
                    attr.color = ide::ide_context::colors::light_blue;
                } else if (code == "lgrey") {
                    attr.color = ide::ide_context::colors::light_grey;
                } else {
                    attr.color = _color;
                    attr.flags = core::font::flags::none;
                    attr.style = core::font::styles::normal;
                }
                ++token;
                continue;
            }
            _document.put(
                    _caret.row(),
                    _caret.column(),
                    core::element_t {static_cast<uint8_t>(*token), attr});

            caret_right();
            ++token;
        }
        caret_down();
        caret_home();
    }

    void console_view::on_execute_command(const execute_command_callable& callable) {
        _execute_command_callback = callable;
    }

    void console_view::on_transition(const core::state_transition_callable& callable) {
        _transition_to_callback = callable;
    }

    bool console_view::transition_to(const std::string& name, const core::parameter_dict& params) {
        bool consumed = false;
        if (_transition_to_callback) {
            consumed = _transition_to_callback(name, params);
        }
        return consumed;
    }

}