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
#include "editor_view.h"

namespace ryu::ide::text_editor {

    editor_view::editor_view(const std::string& name) : core::view(core::view::types::container, name),
                                                       _caret("editor-caret"),
                                                       _header("header-label"),
                                                       _footer("footer-label"),
                                                       _command_line("command-line") {
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

        for_each_selection_char([&](int row, int col) {
            auto element = _document.get(row, col);
            if (element != nullptr)
                element->attr.flags &= ~core::font::flags::reverse;
        });

        _selection.end(_vrow, _vcol);
    }

    void editor_view::on_focus_changed() {
        _caret.enabled(focused());
    }

    void editor_view::update_selection() {
        if (_caret.mode() == core::caret::mode::select) {
            if (_vrow < _selection.start().first && _vcol < _selection.start().second)
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

    core::project* editor_view::project() {
        return _project;
    }

    void editor_view::caret_down(int rows) {
        if (_caret.down(rows))
            scroll_down();
        update_virtual_position();
    }

    hardware::machine* editor_view::machine() {
        return _machine;
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
        _metrics.line_number_width = (5 * font_face()->width) + 2;

        _vcol = 0;
        _vrow = 0;

        _header.palette(palette());
        _header.dock(dock::styles::top);
        _header.font_family(font_family());
        _header.fg_color(ide::ide_context::colors::info_text);
        _header.bg_color(ide::ide_context::colors::fill_color);
        _header.bounds().height(font_face()->line_height);
        _header.margin({_metrics.left_padding, _metrics.right_padding, 5, 0});

        _command_line.palette(palette());
        _command_line.dock(dock::styles::top);
        _command_line.font_family(font_family());
        _command_line.fg_color(ide::ide_context::colors::text);
        _command_line.bg_color(ide::ide_context::colors::fill_color);
        _command_line.on_key_down([&](int keycode) {
            if (keycode == SDLK_ESCAPE) {
                focus(id());
                return false;
            }
            if (keycode == SDLK_RETURN) {
                auto input = _command_line.value();
                if (_execute_command_callable != nullptr) {
                    core::result result;
                    _execute_command_callable(result, input);
                }
                _command_line.clear();
                focus(id());
                return false;
            }
            return true;
        });
        _command_line.initialize(1, 1);
        _command_line.margin({_metrics.left_padding, _metrics.right_padding, 0, 10});

        _footer.palette(palette());
        _footer.dock(dock::styles::bottom);
        _footer.font_family(font_family());
        _footer.fg_color(ide::ide_context::colors::info_text);
        _footer.bg_color(ide::ide_context::colors::fill_color);
        _footer.bounds().height(font_face()->line_height);
        _footer.margin({_metrics.left_padding, _metrics.right_padding, 5, 5});

        _caret.palette(palette());
        _caret.font_family(font_family());
        _caret.padding().left(_metrics.line_number_width);
        _caret.fg_color(ide::ide_context::colors::caret);
        _caret.initialize(0, 0);

        _document.initialize(rows, columns);
        _document.default_attr(core::attr_t {ide::ide_context::colors::text, core::font::styles::normal});
        _document.clear();

        add_child(&_header);
        add_child(&_command_line);
        add_child(&_footer);
        add_child(&_caret);

        dock(dock::styles::fill);
        margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
    }

    void editor_view::on_draw(SDL_Renderer* renderer) {
        auto bounds = client_bounds();
        auto pal = *palette();

        auto& info_text_color = pal[ide::ide_context::colors::info_text];

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

        // XXX: this may not be correct because the font metrics may change
        auto face = font_face();
        auto y = bounds.top();
        auto row_start = _document.row();
        auto row_stop = row_start + _metrics.page_height;
        for (auto row = row_start; row < row_stop; row++) {
            draw_text(renderer, bounds.left(), y, fmt::format("{0:04}", row + 1), info_text_color);

            auto col_start = _document.column();
            auto col_end = col_start + _metrics.page_width;

            auto x = bounds.left() + _caret.padding().left();
            auto chunks = _document.get_line_chunks(row, col_start, col_end);
            for (const auto& chunk : chunks) {
                auto width = static_cast<int32_t>(face->width * chunk.text.length());
                auto color = pal[chunk.attr.color];

                if ((chunk.attr.flags & core::font::flags::reverse) != 0) {
                    push_blend_mode(renderer, SDL_BLENDMODE_BLEND);
                    auto selection_color = pal[ide::ide_context::colors::selection];
                    selection_color.alpha(0x7f);
                    set_color(renderer, selection_color);
                    fill_rect(renderer, core::rect{x, y, width, face->line_height});
                    pop_blend_mode(renderer);
                }

                font_style(chunk.attr.style);
                draw_text(renderer, x, y, chunk.text, color);
                x += width;
            }

            y += face->line_height;
        }
    }

    void editor_view::delete_selection() {
        auto last_row = -1;
        auto target_col = 0;
        for_each_selection_char([&](int row, int col) {
            if (last_row != row) {
                target_col = col;
                last_row = row;
                if (target_col == 0)
                    _document.delete_line(row + 1);
            }
            if (target_col > 0)
                _document.shift_line_left(row, target_col);
        });
        _caret.row(_selection.start().first);
        _caret.column(_selection.start().second);
        update_virtual_position();
        end_selection();
    }

    void editor_view::calculate_page_metrics() {
        auto rect = bounds();
        if (rect.empty())
            return;
        _metrics.page_width = rect.width() / font_face()->width;
        _metrics.page_height = rect.height() / font_face()->line_height;
    }

    void editor_view::insert_text(const char* text) {
        const char* c = text;
        while (true) {
            if (*c == '\0')
                break;

            if (*c == '\n') {
                if (_caret.mode() == core::caret::mode::insert)
                    _document.split_line(_vrow, _vcol);
                caret_down();
                caret_home();
            } else {
                if (_caret.mode() == core::caret::mode::insert)
                    _document.shift_line_right(_vrow, _vcol);
                _document.put(
                        _vrow,
                        _vcol,
                        core::element_t {static_cast<uint8_t>(*c), _document.default_attr()});
                caret_right();
            }

            c++;
        }
    }

    void editor_view::project(core::project* value) {
        _project = value;
    }

    void editor_view::machine(hardware::machine* value) {
        _machine = value;
    }

    bool editor_view::on_process_event(const SDL_Event* e) {
        auto ctrl_pressed = (SDL_GetModState() & KMOD_CTRL) != 0;
        auto shift_pressed = (SDL_GetModState() & KMOD_SHIFT) != 0;

        if (e->type == SDL_TEXTINPUT) {
            insert_text(&e->text.text[0]);
        } else if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_c: {
                    if (ctrl_pressed) {
                        std::stringstream stream;
                        get_selected_text(stream);
                        SDL_SetClipboardText(stream.str().c_str());
                    }
                    break;
                }
                case SDLK_v: {
                    if (ctrl_pressed && SDL_HasClipboardText()) {
                        auto text = SDL_GetClipboardText();
                        insert_text(text);
                    }
                    break;
                }
                case SDLK_x: {
                    if (ctrl_pressed) {
                        std::stringstream stream;
                        get_selected_text(stream);
                        SDL_SetClipboardText(stream.str().c_str());
                        delete_selection();
                    }
                    break;
                }
                case SDLK_ESCAPE: {
                    if (ctrl_pressed) {
                        focus(_command_line.id());
                        return true;
                    }
                    break;
                }
                case SDLK_RETURN: {
                    if (_caret.mode() == core::caret::mode::insert)
                        _document.split_line(_vrow, _vcol);
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
                            _document.put(_vrow, col, core::element_t {0, _document.default_attr()});
                        caret_right(spaces);
                    }
                    return true;
                }
                case SDLK_DELETE: {
                    if (_selection.valid()) {
                        delete_selection();
                    } else {
                        if (_document.is_line_empty(_vrow)) {
                            _document.delete_line(_vrow + 1);
                        } else {
                            _document.shift_line_left(_vrow, _vcol);
                        }
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
                    if (shift_pressed) {
                        update_selection();
                        auto line_end = _document.find_line_end(_vrow);
                        for (auto col = _vcol; col < line_end; col++) {
                            auto element = _document.get(_vrow, col);
                            element->attr.flags |= core::font::flags::reverse;
                        }
                    } else {
                        end_selection();
                    }
                    caret_up();
                    if (shift_pressed)
                        caret_home();
                    return true;
                }
                case SDLK_DOWN: {
                    if (shift_pressed) {
                        update_selection();
                        auto line_end = _document.find_line_end(_vrow);
                        for (auto col = _vcol; col < line_end; col++) {
                            auto element = _document.get(_vrow, col);
                            element->attr.flags |= core::font::flags::reverse;
                        }
                    } else {
                        end_selection();
                    }
                    caret_down();
                    if (shift_pressed)
                        caret_home();
                    return true;
                }
                case SDLK_LEFT: {
                    if (shift_pressed) {
                        update_selection();
                        auto element = _document.get(_vrow, _vcol);
                        element->attr.flags |= core::font::flags::reverse;
                    } else {
                        end_selection();
                    }
                    caret_left();
                    return true;
                }
                case SDLK_RIGHT: {
                    if (shift_pressed) {
                        update_selection();
                        auto element = _document.get(_vrow, _vcol);
                        element->attr.flags |= core::font::flags::reverse;
                    } else {
                        end_selection();
                    }
                    caret_right();
                    return true;
                }
                case SDLK_HOME: {
                    if (shift_pressed) {
                        update_selection();
                        for (auto col = _vcol; col >= 0; col--) {
                            auto element = _document.get(_vrow, col);
                            element->attr.flags |= core::font::flags::reverse;
                        }
                    } else {
                        end_selection();
                    }
                    if (ctrl_pressed)
                        first_page();
                    else
                        caret_home();
                    return true;
                }
                case SDLK_END: {
                    if (shift_pressed) {
                        update_selection();
                        auto line_end = _document.find_line_end(_vrow);
                        for (auto col = _vcol; col < line_end; col++) {
                            auto element = _document.get(_vrow, col);
                            element->attr.flags |= core::font::flags::reverse;
                        }
                    } else {
                        end_selection();
                    }
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

    void editor_view::on_resize(const core::rect& context_bounds) {
        core::view::on_resize(context_bounds);

        calculate_page_metrics();

        _command_line.size(1, _metrics.page_width);
        _caret.page_size(_metrics.page_height, _metrics.page_width);
        _document.page_size(_metrics.page_height, _metrics.page_width);

        if (_caret.row() > _metrics.page_height)
            _caret.row(_metrics.page_height - 1);

        if (_caret.column() > _metrics.page_width)
            _caret.column(_metrics.page_width - 1);

        update_virtual_position();
    }

    void editor_view::get_selected_text(std::stringstream& stream) {
        auto last_row = 0;
        for_each_selection_char([&](int row, int col) {
            if (last_row != row) {
                last_row = row;
                stream << "\n";
            }
            auto element = _document.get(row, col);
            if (element->value == 0)
                stream << " ";
            else
                stream << element->value;
        });
    }

    void editor_view::on_execute_command(const execute_command_callable& callable) {
        _execute_command_callable = callable;
    }

    void editor_view::on_transition(const core::state_transition_callable& callable) {
        _transition_callable = callable;
    }

    void editor_view::for_each_selection_char(const editor_view::char_action_callable& action) {
        _selection.normalize();
        auto row = _selection.start().first;
        auto col = _selection.start().second;
        auto line_end = _document.find_line_end(row);
        while (true) {
            action(row, col);
            col++;
            if (col >= line_end
            || (_selection.end().second > 0 && col > _selection.end().second && row == _selection.end().first)) {
                col = 0;
                row++;
                if (row > _selection.end().first)
                    break;
                line_end = _document.find_line_end(row);
            }
        }
    }

}