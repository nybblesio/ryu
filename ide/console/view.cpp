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
#include <ide/context.h>
#include "view.h"

namespace ryu::ide::console {

    view::view(
            core::context* context,
            core::view* parent,
            int id,
            const std::string& name) : core::view(context, parent, core::view::types::custom, id, name),
                                       _header(context, this, ids::header_label, "header-label"),
                                       _footer(context, this, ids::footer_label, "footer-label"),
                                       _caret(context, this, ids::caret, "console-caret") {
    }

    view::~view() {
    }

    void view::caret_end() {
        _caret.column(_page_width);
    }

    void view::caret_home() {
        _caret.column(0);
    }

    void view::caret_up(int rows) {
        _caret.up(rows);
    }

    void view::caret_down(int rows) {
        if (_caret.down(rows))
            _document.shift_up();
    }

    bool view::caret_left(int columns) {
        if (_caret.left(columns)) {
            caret_up();
            caret_end();
            return true;
        }
        return false;
    }

    bool view::caret_right(int columns) {
        if (_caret.right(columns)) {
            caret_down();
            caret_home();
            return true;
        }
        return false;
    }

    void view::initialize() {
        auto clip_rect = context()->clip_rect();

        const int left_padding = 10;
        const int right_padding = 10;
        const int footer_padding = font()->line_height;
        const int header_padding = font()->line_height * 2;

        _page_width = static_cast<short>((clip_rect.w - (left_padding + right_padding)) / font()->width);
        _page_height = static_cast<short>((clip_rect.h - (header_padding + footer_padding + 20)) / font()->line_height);

        _document.initialize(_page_height, _page_width, _page_width, _page_height);
        _document.clear();

        _header.rect()
                .pos(0, 0)
                .size(clip_rect.w, header_padding);
        _header.font(font());
        _header.bg_color(ide::context::colors::fill_color);
        _header.fg_color(ide::context::colors::info_text);
        _header.padding({left_padding, right_padding, 5, 0});

        _footer.rect()
                .pos(0, clip_rect.h - (footer_padding + 10))
                .size(clip_rect.w, footer_padding);
        _footer.font(font());
        _footer.bg_color(ide::context::colors::fill_color);
        _footer.fg_color(ide::context::colors::info_text);
        _footer.padding({left_padding, right_padding, 0, 0});

        _caret.initialize(0, 0, _page_width, _page_height);
        _caret.fg_color(ide::context::colors::caret);
        _caret.font(font());

        rect({0, 0, clip_rect.w, clip_rect.h});
        padding({left_padding, right_padding, header_padding + 5, footer_padding});
    }

    void view::on_draw(SDL_Renderer* renderer) {
        auto bounds = client_rect();
        auto& white = (*context()->palette())[ide::context::colors::text];
        auto& grey = (*context()->palette())[ide::context::colors::info_text];

        auto text_color = white.to_sdl_color();

        std::string project_name = "(none)";
        std::string machine_name = "(none)";
        auto project = dynamic_cast<ide::context*>(context())->project();
        if (project != nullptr) {
            project_name = project->name();
            machine_name = project->machine()->name();
        }
        _header.value(fmt::format("project: {0} | machine: {1}", project_name, machine_name));
        _footer.value(fmt::format(
                "X:{0:03d} Y:{1:02d} | {2}",
                _caret.column() + 1,
                _caret.row() + 1,
                _caret.mode() == core::caret::mode::overwrite ? "OVR" : "INS"));

        auto y = bounds.top();
        for (auto row = 0; row < _page_height; row++) {
            std::stringstream stream;
            _document.write_line(stream, row, 0, _page_width);

            FC_DrawColor(
                    font()->glyph,
                    renderer,
                    bounds.left(),
                    y,
                    text_color,
                    stream.str().c_str());

            y += font()->line_height;
        }
    }

    bool view::on_process_event(const SDL_Event* e) {
        auto ctrl_pressed = (SDL_GetModState() & KMOD_CTRL) != 0;
        auto shift_pressed = (SDL_GetModState() & KMOD_SHIFT) != 0;
        auto mode = _caret.mode();

        if (e->type == SDL_TEXTINPUT) {
            if (mode == core::caret::mode::insert) {
                _document.shift_right(_caret.row(), _caret.column());
            }
            const char* c = &e->text.text[0];
            while (*c != '\0') {
                _document.put(_caret.row(), _caret.column(), static_cast<uint8_t>(*c));
                if (caret_right()) {
                    caret_right();
                }
                c++;
            }
        } else if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_ESCAPE: {
                    if (_transition_to_callback != nullptr)
                        return _transition_to_callback("text_editor");
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
                            auto value = _document.get(_caret.row(), _caret.column());
                            if (value == 0)
                                break;
                        }

                        // second, scan forward to the next null value to build the command
                        caret_right();
                        while (true) {
                            caret_right();
                            auto value = _document.get(_caret.row(), _caret.column());
                            if (value == 0)
                                break;
                            cmd << value;
                        }

                        // finally, if we got something, try to execute it
                        auto str = cmd.str();
                        if (str.length() > 0 && _execute_command_callback != nullptr) {
                            _execute_command_callback(result, str);

                            caret_down();
                            caret_home();

                            for (const auto& msg : result.messages())
                                write_message(fmt::format("{0}{1}", msg.is_error() ? "ERROR: " : "", msg.message()));

                            caret_down();

                            auto consumed = true;

                            if (result.has_code("C001"))
                                context()->engine()->quit();
                            else if (result.has_code("C002")) {
                                if (_transition_to_callback)
                                    consumed = _transition_to_callback("text_editor");
                            }
                            else if (result.has_code("C004")) {
                                _document.clear();
                                caret_home();
                                _caret.row(0);
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
                        _caret.row(_page_height - 1);
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

    void view::write_message(const std::string& message) {
        caret_home();
        for (const auto& c : message) {
            if (c == '\n') {
                caret_down();
                caret_home();
                continue;
            }
            _document.put(_caret.row(), _caret.column(), static_cast<uint8_t>(c));
            caret_right();
        }
        caret_down();
        caret_home();
    }

    void view::on_transition(const transition_to_callable& callable) {
        _transition_to_callback = callable;
    }

    void view::on_execute_command(const execute_command_callable& callable) {
        _execute_command_callback = callable;
    }

}