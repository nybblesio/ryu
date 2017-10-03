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
#include <ide/ide_types.h>
#include "editor_view.h"

namespace ryu::ide::text_editor {

    editor_view::editor_view(const std::string& name) : core::view(core::view::types::container, name),
                                                       _caret("editor-caret") {
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
        caret_home();
        for (auto i = 0; i < end_column; i++)
            caret_right();
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

        for_each_selection_char([&](uint32_t row, uint16_t col) {
            auto element = _document.get(row, col);
            if (element != nullptr)
                element->attr.flags &= ~core::font::flags::reverse;
        });

        _selection.end(_vrow, _vcol);
    }

    void editor_view::on_focus_changed() {
        _caret.enabled(focused());
    }

    void editor_view::update_selection(uint16_t line_end) {
        if (_caret.mode() == core::caret::mode::select) {
            if (_vrow < _selection.start().row && line_end < _selection.start().column)
                _selection.start(_vrow, line_end);
            else
                _selection.end(_vrow, line_end);
        } else {
            _caret.select();
            _selection.start(_vrow, 0);
            _selection.end(_vrow, line_end);
        }
    }

    void editor_view::goto_line(uint32_t row) {
        if (row < 1)
            row = 1;
        else if (row > _document.rows())
            row = _document.rows();
        _document.row(row - 1);
        update_virtual_position();
    }

    void editor_view::caret_up(uint8_t rows) {
        if (_caret.up(rows))
            scroll_up();
        update_virtual_position();
    }

    core::project* editor_view::project() {
        return _project;
    }

    void editor_view::caret_down(uint8_t rows) {
        if (_caret.down(rows))
            scroll_down();
        update_virtual_position();
    }

    void editor_view::raise_caret_changed() {
        if (_caret_changed_callback != nullptr)
            _caret_changed_callback(_caret);
    }

    hardware::machine* editor_view::machine() {
        return _machine;
    }

    void editor_view::caret_left(uint8_t columns) {
        if (_caret.left(columns))
            scroll_left();
        update_virtual_position();
    }

    bool editor_view::caret_right(uint8_t columns) {
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

    void editor_view::raise_document_changed() {
        if (_document_changed_callback != nullptr)
            _document_changed_callback(_document);
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

    void editor_view::initialize(uint32_t rows, uint16_t columns) {
        _metrics.line_number_width = font_face()->measure_chars(5) + 2;

        _vcol = 0;
        _vrow = 0;

        _caret.palette(palette());
        _caret.font_family(font_family());
        _caret.fg_color(ide::colors::caret);
        _caret.padding().left(_metrics.line_number_width);
        _caret.on_caret_changed([&]() {
            raise_caret_changed();
        });
        _caret.initialize(0, 0);

        _document.on_document_changed([&]() {
            raise_document_changed();
        });
        _document.default_attr(core::attr_t {ide::colors::text, core::font::styles::normal});
        _document.document_size(rows, columns);
        _document.clear();
        add_child(&_caret);
        margin({_metrics.left_padding, _metrics.right_padding, 5, 5});
    }

    void editor_view::on_draw(core::renderer& surface) {
        auto bounds = client_bounds();
        auto pal = *palette();

        auto& info_text_color = pal[ide::colors::info_text];

        // XXX: this may not be correct because the font metrics may change
        auto face = font_face();
        auto y = bounds.top();
        auto row_start = _document.row();
        auto row_stop = row_start + _metrics.page_height;
        for (auto row = row_start; row < row_stop; row++) {
            surface.draw_text(font_face(), bounds.left(), y, fmt::format("{0:04}", row + 1), info_text_color);

            uint16_t col_start = _document.column();
            uint16_t col_end = col_start + _metrics.page_width;

            auto x = bounds.left() + _caret.padding().left();
            auto chunks = _document.get_line_chunks(row, col_start, col_end);
            for (const auto& chunk : chunks) {
                auto width = static_cast<int32_t>(face->width * chunk.text.length());
                auto color = pal[chunk.attr.color];

                if ((chunk.attr.flags & core::font::flags::reverse) != 0) {
                    surface.push_blend_mode(SDL_BLENDMODE_BLEND);
                    auto selection_color = pal[ide::colors::selection];
                    selection_color.alpha(0x7f);
                    surface.set_color(selection_color);
                    surface.fill_rect(core::rect{x, y, width, face->line_height});
                    surface.pop_blend_mode();
                }

                font_style(chunk.attr.style);
                surface.draw_text(font_face(), x, y, chunk.text, color);
                x += width;
            }

            y += face->line_height;
        }
    }

    void editor_view::delete_selection() {
        _selection.normalize();

        auto row = _selection.start().row;
        auto last_row = _selection.end().row;

        if (row == last_row) {
            auto line_end = _document.find_line_end(row);
            if (_selection.start().column == 0 && _selection.end().column == line_end) {
                _document.delete_line(row);
            } else {
                for (uint16_t col = _selection.start().column; col < _selection.end().column; col++)
                    _document.put(row, col, core::element_t {0, _document.default_attr()});
            }
        } else {
            _document.delete_line(row);
            for (auto i = row; i < last_row; i++)
                _document.delete_line(row);
            for (uint16_t col = 0; col < _selection.end().column; col++)
                _document.put(row, col, core::element_t {0, _document.default_attr()});
        }

        _caret.row(static_cast<uint8_t>(_selection.start().row));
        _caret.column(static_cast<uint8_t>(_selection.start().column));
        update_virtual_position();
        end_selection();
    }

    void editor_view::calculate_page_metrics() {
        auto rect = bounds();
        if (rect.empty())
            return;
        _metrics.page_width = static_cast<uint8_t>((rect.width() - _metrics.line_number_width) / font_face()->width);
        _metrics.page_height = static_cast<uint8_t>(rect.height() / font_face()->line_height);
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
                        auto spaces = static_cast<uint8_t>(4 - (_vcol % 4));
                        caret_left(spaces);
                        _document.shift_line_left(_vrow, _vcol, spaces);
                    } else {
                        auto spaces = static_cast<uint8_t>(4 - (_vcol % 4));
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
                            _document.delete_line(_vrow);
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
                        auto line_end = _document.find_line_end(_vrow);
                        update_selection(line_end);
                        for (auto col = _vcol; col < line_end; col++) {
                            auto element = _document.get(_vrow, col);
                            if (element != nullptr)
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
                        auto line_end = _document.find_line_end(_vrow);
                        update_selection(line_end);
                        for (auto col = _vcol; col < line_end; col++) {
                            auto element = _document.get(_vrow, col);
                            if (element != nullptr)
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
                        update_selection(_vcol);
                        auto element = _document.get(_vrow, _vcol);
                        if (element != nullptr)
                            element->attr.flags |= core::font::flags::reverse;
                    } else {
                        end_selection();
                    }
                    caret_left();
                    return true;
                }
                case SDLK_RIGHT: {
                    if (shift_pressed) {
                        update_selection(_vcol);
                        auto element = _document.get(_vrow, _vcol);
                        if (element != nullptr)
                            element->attr.flags |= core::font::flags::reverse;
                    } else {
                        end_selection();
                    }
                    caret_right();
                    return true;
                }
                case SDLK_HOME: {
                    if (shift_pressed) {
                        update_selection(_vcol);
                        for (auto col = _vcol; col >= 0; col--) {
                            auto element = _document.get(_vrow, col);
                            if (element != nullptr)
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
                        auto line_end = _document.find_line_end(_vrow);
                        update_selection(line_end);
                        for (auto col = _vcol; col < line_end; col++) {
                            auto element = _document.get(_vrow, col);
                            if (element != nullptr)
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

        _caret.page_size(_metrics.page_height, _metrics.page_width);
        _document.page_size(_metrics.page_height, _metrics.page_width);

        update_virtual_position();
    }

    void editor_view::get_selected_text(std::stringstream& stream) {
        _selection.normalize();
        auto row = _selection.start().row;
        auto last_row = _selection.end().row;

        if (row == last_row) {
            _document.write_line(stream, row, _selection.start().column, _selection.end().column);
            stream << "\n";
        } else {
            _document.write_line(stream, row, _selection.start().column, _document.columns());
            stream << "\n";
            ++row;
            if (row == last_row) {
                _document.write_line(stream, last_row, 0, _selection.end().column);
                stream << "\n";
            } else {
                for (; row < last_row; ++row) {
                    _document.write_line(stream, row, 0, _document.columns());
                    stream << "\n";
                }
                _document.write_line(stream, last_row, 0, _selection.end().column);
                stream << "\n";
            }
        }
    }

    void editor_view::on_caret_changed(const caret_changed_callable& callable) {
        _caret_changed_callback = callable;
    }

    void editor_view::for_each_selection_char(const editor_view::char_action_callable& action) {
        _selection.normalize();
        auto row = _selection.start().row;
        auto col = _selection.start().column;
        auto line_end = _document.find_line_end(row);
        while (true) {
            action(row, col);
            col++;
            if (col >= line_end
            || (_selection.end().column > 0 && col > _selection.end().column && row == _selection.end().row)) {
                col = 0;
                row++;
                if (row > _selection.end().row)
                    break;
                line_end = _document.find_line_end(row);
            }
        }
    }

    void editor_view::on_document_changed(const editor_view::document_changed_callable& callable) {
        _document_changed_callback = callable;
    }

}