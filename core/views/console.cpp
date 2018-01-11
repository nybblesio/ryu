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
#include <core/text_formatter.h>
#include "console.h"

namespace ryu::core {

    console::console(const std::string& name) : core::view(core::view::types::container, name),
                                                          _caret("console-caret") {
    }

    void console::caret_end() {
        _caret.column(_metrics.page_width);
    }

    void console::caret_home() {
        _caret.column(0);
    }

    void console::initialize() {
        _color = fg_color();

        _document.default_attr(core::attr_t {_color, core::font::styles::normal, core::font::flags::none});
        _document.document_size(128, 128);
        _document.clear();

        _caret.overwrite();
        _caret.initialize(0, 0);
        _caret.palette(palette());
        _caret.font_family(font_family());
        _caret.on_caret_changed([&]() {
            raise_caret_changed();
        });

        add_child(&_caret);
        margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
    }

    void console::raise_caret_changed() {
        if (_caret_changed_callback != nullptr)
            _caret_changed_callback(_caret);
    }

    void console::caret_up(uint8_t rows) {
        _caret.up(rows);
    }

    void console::caret_down(uint8_t rows) {
        if (_caret.down(rows))
            _document.shift_up();
    }

    void console::calculate_page_metrics() {
        auto rect = bounds();
        if (rect.empty())
            return;
        _metrics.page_width = static_cast<uint8_t>(rect.width() / font_face()->width);
        _metrics.page_height = static_cast<uint8_t>(rect.height() / font_face()->line_height);
    }

    void console::caret_color(uint8_t color) {
        _caret.fg_color(color);
    }

    bool console::caret_left(uint8_t columns) {
        if (_caret.left(columns)) {
            caret_up();
            caret_end();
            return true;
        }
        return false;
    }

    bool console::caret_right(uint8_t columns) {
        if (_caret.right(columns)) {
            caret_down();
            caret_home();
            return true;
        }
        return false;
    }

    void console::code_mapper(const code_to_attr_dict& value) {
        _code_mapper = value;
    }

    void console::on_resize(const core::rect& context_bounds) {
        core::view::on_resize(context_bounds);
        calculate_page_metrics();
        _caret.page_size(_metrics.page_height, _metrics.page_width);
        _document.page_size(_metrics.page_height, _metrics.page_width);
    }

    void console::on_draw(core::renderer& surface) {
        auto bounds = client_bounds();
        auto pal = *palette();

        // XXX: this may not be correct because font metrics may change
        auto face = font_face();
        auto y = bounds.top();
        for (uint32_t row = 0; row < _metrics.page_height; row++) {
            auto x = bounds.left();
            auto chunks = _document.get_line_chunks(row, 0, _metrics.page_width);
            for (const auto& chunk : chunks) {
                auto width = static_cast<int32_t>(face->width * chunk.text.length());
                auto color = pal[chunk.attr.color];

                if ((chunk.attr.flags & core::font::flags::reverse) != 0) {
                    surface.set_color(color);
                    color = pal[bg_color()];
                    surface.fill_rect(core::rect{x, y, width, face->line_height});
                }

                font_style(chunk.attr.style);
                surface.draw_text(font_face(), x, y, chunk.text, color);
                x += width;
            }
            y += face->line_height;
        }
    }

    bool console::on_process_event(const SDL_Event* e) {
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
                case SDLK_1: {
                    if (ctrl_pressed) {
                        core::attr_t a {};
                        auto it = _code_mapper.find("black");
                        if (it != _code_mapper.end()) {
                            it->second(a);
                            _color = a.color;
                            return true;
                        }
                    }
                    break;
                }
                case SDLK_2: {
                    if (ctrl_pressed) {
                        core::attr_t a {};
                        auto it = _code_mapper.find("white");
                        if (it != _code_mapper.end()) {
                            it->second(a);
                            _color = a.color;
                            return true;
                        }
                    }
                    break;
                }
                case SDLK_3: {
                    if (ctrl_pressed) {
                        core::attr_t a {};
                        auto it = _code_mapper.find("red");
                        if (it != _code_mapper.end()) {
                            it->second(a);
                            _color = a.color;
                            return true;
                        }
                    }
                    break;
                }
                case SDLK_4: {
                    if (ctrl_pressed) {
                        core::attr_t a {};
                        auto it = _code_mapper.find("cyan");
                        if (it != _code_mapper.end()) {
                            it->second(a);
                            _color = a.color;
                            return true;
                        }
                    }
                    break;
                }
                case SDLK_5: {
                    if (ctrl_pressed) {
                        core::attr_t a {};
                        auto it = _code_mapper.find("purple");
                        if (it != _code_mapper.end()) {
                            it->second(a);
                            _color = a.color;
                            return true;
                        }
                    }
                    break;
                }
                case SDLK_6: {
                    if (ctrl_pressed) {
                        core::attr_t a {};
                        auto it = _code_mapper.find("green");
                        if (it != _code_mapper.end()) {
                            it->second(a);
                            _color = a.color;
                            return true;
                        }
                    }
                    break;
                }
                case SDLK_7: {
                    if (ctrl_pressed) {
                        core::attr_t a {};
                        auto it = _code_mapper.find("blue");
                        if (it != _code_mapper.end()) {
                            it->second(a);
                            _color = a.color;
                            return true;
                        }
                    }
                    break;
                }
                case SDLK_8: {
                    if (ctrl_pressed) {
                        core::attr_t a {};
                        auto it = _code_mapper.find("yellow");
                        if (it != _code_mapper.end()) {
                            it->second(a);
                            _color = a.color;
                            return true;
                        }
                    }
                    break;
                }
                case SDLK_9: {
                    break;
                }
                case SDLK_ESCAPE: {
                    return transition_to("source_editor", {});
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
                                auto error_part = msg.is_error() ? "<bold><red>ERROR:<> " : "";
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
                                    consumed = transition_to("source_editor", code->params());
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
                                            consumed = transition_to("source_editor", params);
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
                        _caret.row(static_cast<uint8_t>(_metrics.page_height - 1));
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

    void console::write_message(const std::string& message) {
        core::attr_t attr {
            _color,
            core::font::styles::normal,
            core::font::flags::none
        };

        // XXX: should implement MORE functionality, like unix more or less
        caret_home();

        auto formatted_text = core::text_formatter::format_text(message);
        for (const auto& span : formatted_text.spans) {
            if (span.attr_code == "newline") {
                caret_down();
                caret_home();
                continue;
            }

            auto it = _code_mapper.find(span.attr_code);
            if (it == _code_mapper.end()) {
                attr.color = _color;
                attr.flags = core::font::flags::none;
                attr.style = core::font::styles::normal;
            } else {
                it->second(attr);
            }

            auto token = span.text.begin();
            while (token != span.text.end()) {
                _document.put(
                        _caret.row(),
                        _caret.column(),
                        core::element_t {static_cast<uint8_t>(*token), attr});
                caret_right();
                ++token;
            }
        }

        caret_down();
        caret_home();
    }

    void console::on_execute_command(const execute_command_callable& callable) {
        _execute_command_callback = callable;
    }

    void console::on_transition(const core::state_transition_callable& callable) {
        _transition_to_callback = callable;
    }

    void console::on_caret_changed(const console::caret_changed_callable& callable) {
        _caret_changed_callback = callable;
    }

    bool console::transition_to(const std::string& name, const core::parameter_dict& params) {
        bool consumed = false;
        if (_transition_to_callback) {
            consumed = _transition_to_callback(name, params);
        }
        return consumed;
    }

}