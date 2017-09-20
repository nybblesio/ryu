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
#include "editor_view.h"

namespace ryu::ide::text_editor {

    editor_view::editor_view(
            core::context* context,
            core::view* parent,
            int id,
            const std::string& name) : core::view(context, parent, core::view::types::custom, id, name),
                                       _caret(context, this, ids::caret, "editor-caret"),
                                       _header(context, this, ids::header_label, "header-label"),
                                       _footer(context, this, ids::footer_label, "footer-label"),
                                       _command_line(context, this, ids::command_line, "command-line") {
    }

    editor_view::~editor_view() {
    }

    void editor_view::clear() {
        _document.clear();
        first_page();
        _caret.row(0);
        _caret.column(0);
        update_virtual_position();
    }

    void editor_view::page_up() {
        _document.page_up();
        update_virtual_position();
    }

    void editor_view::page_down() {
        _document.page_down();
        update_virtual_position();
    }

    void editor_view::scroll_up() {
        _document.scroll_up();
        update_virtual_position();
    }

    void editor_view::caret_end() {
        auto end_column = _document.find_line_end(_vrow);
        _caret.column(end_column);
        update_virtual_position();
    }

    void editor_view::last_page() {
        _document.last_page();
        update_virtual_position();
    }

    void editor_view::first_page() {
        _document.first_page();
        update_virtual_position();
    }

    void editor_view::caret_home() {
        _caret.column(0);
        _document.home();
        update_virtual_position();
    }

    void editor_view::scroll_down() {
        _document.scroll_down();
        update_virtual_position();
    }

    void editor_view::scroll_left() {
        _document.scroll_left();
        update_virtual_position();
    }

    bool editor_view::scroll_right() {
        bool clamped = _document.scroll_right();
        update_virtual_position();
        return clamped;
    }

    void editor_view::end_selection() {
        if (_caret.mode() != core::caret::mode::select)
            return;

        _caret.insert();
        _selection.end(_vrow, _vcol);
    }

    void editor_view::on_focus_changed() {
        _caret.enabled(focused());
    }

    void editor_view::update_selection() {
        if (_caret.mode() == core::caret::mode::select) {
            if (_vrow < _selection.start().first || _vcol < _selection.start().second)
                _selection.start(_vrow, _vcol);
            else
                _selection.end(_vrow, _vcol);
        } else {
            _caret.select();
            _selection.start(_vrow, _vcol);
            _selection.end(_vrow, _vcol);
        }
    }

    void editor_view::goto_line(int row) {
        if (row < 1)
            row = 1;
        else if (row > _document.rows())
            row = _document.rows();
        _document.row(row - 1);
        update_virtual_position();
    }

    void editor_view::caret_up(int rows) {
        if (_caret.up(rows))
            scroll_up();
        update_virtual_position();
    }

    void editor_view::caret_down(int rows) {
        if (_caret.down(rows))
            scroll_down();
        update_virtual_position();
    }

    void editor_view::caret_left(int columns) {
        if (_caret.left(columns))
            scroll_left();
        update_virtual_position();
    }

    bool editor_view::caret_right(int columns) {
        auto clamped = false;
        if (_caret.right(columns)) {
            clamped = scroll_right();
            if (clamped) {
                caret_down();
                caret_home();
            }
        }
        update_virtual_position();
        return clamped;
    }

    void editor_view::load(const fs::path& path) {
        _document.load(path);
    }

    void editor_view::save(const fs::path& path) {
        _document.save(path);
    }

    void editor_view::update_virtual_position() {
        _vrow = _document.row() + _caret.row();
        _vcol = _document.column() + _caret.column();

        if (_caret.mode() != core::caret::mode::select)
            _selection.clear();
    }

    void editor_view::find(const std::string& needle) {
    }

    void editor_view::initialize(int rows, int columns) {
        auto clip_rect = context()->clip_rect();

        const int left_padding = 10;
        const int right_padding = 15;
        const int line_number_width = 6 * font_face()->width;
        const int footer_padding = font_face()->line_height * 2;
        const int header_padding = (font_face()->line_height * 2) + 25;

        _vcol = 0;
        _vrow = 0;
        _page_width = (clip_rect.w - (left_padding + line_number_width + right_padding)) / font_face()->width;
        _page_height = ((clip_rect.h - (header_padding + footer_padding)) / font_face()->line_height);

        _header.rect()
                .pos(0, 0)
                .size(clip_rect.w, font_face()->line_height * 2);
        _header.font_family(font_family());
        _header.bg_color(ide::context::colors::fill_color);
        _header.fg_color(ide::context::colors::info_text);
        _header.padding({left_padding, right_padding, 5, 0});

        _command_line.fg_color(ide::context::colors::text);
        _command_line.font_family(font_family());
        _command_line.initialize(1, _page_width);
        _command_line.rect().pos(left_padding, font_face()->line_height + 3);
        _command_line.on_key_down([&](int keycode) {
            if (keycode == SDLK_ESCAPE) {
                focus(id());
                return;
            }
            if (keycode == SDLK_RETURN) {
                auto input = _command_line.value();
                if (_execute_command_callable != nullptr) {
                    core::result result;
                    _execute_command_callable(result, input);
                }
                _command_line.clear();
                focus(id());
            }
        });

        _footer.rect()
                .pos(0, clip_rect.h - (footer_padding + 10))
                .size(clip_rect.w, footer_padding);
        _footer.font_family(font_family());
        _footer.bg_color(ide::context::colors::fill_color);
        _footer.fg_color(ide::context::colors::info_text);
        _footer.padding({left_padding, right_padding, 13, 0});

        _caret.font_family(font_family());
        _caret.padding().left(line_number_width);
        _caret.fg_color(ide::context::colors::caret);
        _caret.initialize(0, 0, _page_width, _page_height);

        _document.initialize(rows, columns, _page_width, _page_height);
        _document.clear();

        rect({0, 0, clip_rect.w, clip_rect.h});
        padding({left_padding, right_padding, header_padding, footer_padding});
    }

    void editor_view::on_draw() {
        auto bounds = client_rect();

        auto& text_color = (*context()->palette())[ide::context::colors::text];
        auto& info_text_color = (*context()->palette())[ide::context::colors::info_text];

        std::string cpu_name = "(none)";
        std::string file_name = _document.filename();
        if (file_name.empty()) {
            file_name = "(none)";
        }

        _header.value(fmt::format("cpu: {0} | file: {1}", cpu_name, file_name));
        _footer.value(fmt::format(
                "C:{0:03d}/{1:03d} R:{2:04d}/{3:04d} | X:{4:03d} Y:{5:02d} | {6}",
                _document.column() + 1,
                _document.columns(),
                _document.row() + 1,
                _document.rows(),
                _caret.column() + 1,
                _caret.row() + 1,
                _caret.mode() == core::caret::mode::overwrite ? "OVR" : "INS"));

        auto y = bounds.top();
        auto row_start = _document.row();
        auto row_stop = row_start + _page_height;
        for (auto row = row_start; row < row_stop; row++) {
            draw_text(bounds.left(), y, fmt::format("{0:04}", row + 1), info_text_color);

            auto col_start = _document.column();
            auto col_end = col_start + _page_width;
            std::stringstream stream;
            _document.write_line(stream, row, col_start, col_end);

            draw_text(bounds.left() + _caret.padding().left(), y, stream.str(), text_color);

            if (_selection.selected(row, 0)) {
                push_blend_mode(SDL_BLENDMODE_BLEND);
                auto selection_color = (*context()->palette())[ide::context::colors::selection];
                selection_color.alpha(0x7f);
                set_color(selection_color);
                fill_rect(core::rect{
                        bounds.left() + _caret.padding().left(),
                        y,
                        font_face()->width * (_page_width + 1),
                        font_face()->line_height
                });
                pop_blend_mode();
            }

            y += font_face()->line_height;
        }
    }

    bool editor_view::on_process_event(const SDL_Event* e) {
        auto ctrl_pressed = (SDL_GetModState() & KMOD_CTRL) != 0;
        auto shift_pressed = (SDL_GetModState() & KMOD_SHIFT) != 0;

        if (e->type == SDL_TEXTINPUT) {
            if (_caret.mode() == core::caret::mode::insert)
                _document.shift_line_right(_vrow, _vcol);
            const char* c = &e->text.text[0];
            while (*c != '\0') {
                _document.put(_vrow, _vcol, static_cast<uint8_t>(*c));
                caret_right();
                c++;
            }
        } else if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_ESCAPE: {
                    if (ctrl_pressed) {
                        focus(ids::command_line);
                        return true;
                    }
                    context()->pop_state();
                    return true;
                }
                case SDLK_RETURN: {
                    if (_caret.mode() == core::caret::mode::insert) {
                        auto row = _vrow;
                        if (_caret.column() == 0)
                            row--;
                        if (row < 0)
                            row = 0;
                        _document.insert_line(row);
                    }
                    caret_down();
                    caret_home();
                    return true;
                }
                case SDLK_TAB: {
                    if (shift_pressed) {
                        if (_caret.column() == 0)
                            return true;
                        auto spaces = core::document::spaces_to_prev_tabstop(_vcol, 4);
                        caret_left(spaces);
                        _document.shift_line_left(_vrow, _vcol, spaces);
                    } else {
                        auto spaces = core::document::spaces_to_next_tabstop(_vcol, 4);
                        _document.shift_line_right(_vrow, _vcol, spaces);
                        for (auto col = _vcol; col < _vcol + spaces; col++)
                            _document.put(_vrow, col, 0);
                        caret_right(spaces);
                    }
                    return true;
                }
                case SDLK_DELETE: {
                    if (_document.is_line_empty(_vrow)) {
                        _document.delete_line(_vrow + 1);
                    } else {
                        _document.shift_line_left(_vrow, _vcol);
                    }
                    return true;
                }
                case SDLK_BACKSPACE: {
                    if (_document.is_line_empty(_vrow)) {
                        _document.delete_line(_vrow + 1);
                    } else if (_caret.column() == 0) {
                        _document.delete_line(_vrow);
                        caret_up();
                    } else {
                        caret_left();
                        _document.shift_line_left(_vrow, _vcol);
                    }
                    return true;
                }
                case SDLK_UP: {
                    if (shift_pressed)
                        update_selection();
                    else
                        end_selection();

                    caret_up();

                    return true;
                }
                case SDLK_DOWN: {
                    if (shift_pressed)
                        update_selection();
                    else
                        end_selection();

                    caret_down();

                    return true;
                }
                case SDLK_LEFT: {
                    if (shift_pressed)
                        update_selection();
                    else
                        end_selection();

                    caret_left();

                    return true;
                }
                case SDLK_RIGHT: {
                    if (shift_pressed)
                        update_selection();
                    else
                        end_selection();

                    caret_right();

                    return true;
                }
                case SDLK_HOME: {
                    if (ctrl_pressed)
                        first_page();
                    else
                        caret_home();
                    return true;
                }
                case SDLK_END: {
                    if (ctrl_pressed)
                        last_page();
                    else
                        caret_end();
                    return true;
                }
                case SDLK_PAGEDOWN: {
                    page_down();
                    return true;
                }
                case SDLK_PAGEUP: {
                    page_up();
                    return true;
                }
                case SDLK_INSERT: {
                    if (_caret.mode() == core::caret::mode::insert)
                        _caret.overwrite();
                    else
                        _caret.insert();
                    return true;
                }
            }
        }
        return false;
    }

    void editor_view::on_transition(const transition_to_callable& callable) {
        _transition_callable = callable;
    }

    void editor_view::on_execute_command(const execute_command_callable& callable) {
        _execute_command_callable = callable;
    }

}