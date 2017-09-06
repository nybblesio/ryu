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
#include <ide/context.h>
#include <common/string_support.h>
#include "controller.h"

namespace ryu::ide::hex_editor {

    controller::controller(
            core::context* context,
            int id,
            const std::string& name) : core::state(context, id, name) {
    }

    controller::~controller() {
//        delete _document;
        delete _caret;
    }

    void controller::page_up() {
//        _document->page_up();
        update_virtual_position();
    }

    void controller::page_down() {
//        _document->page_down();
        update_virtual_position();
    }

    void controller::scroll_up() {
//        _document->scroll_up();
        update_virtual_position();
    }

    void controller::caret_end() {
        _caret->column(_page_width);
//        _document->end();
        update_virtual_position();
    }

    void controller::last_page() {
//        _document->last_page();
        update_virtual_position();
    }

    void controller::first_page() {
//        _document->first_page();
        update_virtual_position();
    }

    void controller::caret_home() {
        _caret->column(0);
//        _document->home();
        update_virtual_position();
    }

    void controller::scroll_down() {
//        _document->scroll_down();
        update_virtual_position();
    }

    void controller::scroll_left() {
//        _document->scroll_left();
        update_virtual_position();
    }

    void controller::scroll_right() {
//        _document->scroll_right();
        update_virtual_position();
    }

    void controller::end_selection() {
        if (_caret->mode() != core::caret::mode::select)
            return;

        _caret->insert();
        _selection_end.first = _vrow;
        _selection_end.second = _vcol;
    }

    void controller::update_selection() {
        if (_caret->mode() == core::caret::mode::select) {
            _selection_end.first = _vrow;
            _selection_end.second = _vcol;
        } else {
            _caret->select();
            _selection_start.first = _vrow;
            _selection_start.second = _vcol;
            _selection_end.first = _vrow;
            _selection_end.second = _vcol;
        }
    }

    void controller::caret_up(int rows) {
        if (_caret->up(rows))
            scroll_up();
        update_virtual_position();
    }

    void controller::caret_down(int rows) {
        if (_caret->down(rows))
            scroll_down();
        update_virtual_position();
    }

    void controller::caret_left(int columns) {
        if (_caret->left(columns))
            scroll_left();
        update_virtual_position();
    }

    void controller::caret_right(int columns) {
        if (_caret->right(columns))
            scroll_right();
        update_virtual_position();
    }

    void controller::on_update(uint32_t dt) {
        auto cmd = ryu::trimmed(context()->blackboard("text-editor-cmd"));
        if (!cmd.empty()) {
            auto tokens = ryu::string_to_list(cmd, ' ');
            if (!tokens.empty()) {
                if (tokens[0] == "goto") {
                    auto line = std::atoi(tokens[1].c_str());
                    if (line < 1)
                        line = 1;
                    else if (line > 8192)
                        line = 8192;
//                    _document->row(line - 1);
                } else if (tokens[0] == "find") {
                    std::string needle;
                    for (size_t i = 1; i < tokens.size(); i++)
                        needle += tokens[i];
                    auto start_col = _vcol;
//                    for (auto row = _vrow; row < document::max_rows; row++) {
//                        std::stringstream stream;
//                        for (auto col = start_col; col < line::max_columns; col++) {
//                            auto value = _document->get(row, col);
//                            if (value == 0)
//                                stream << " ";
//                            else
//                                stream << value;
//                        }
//
//                        auto index = stream.str().find(needle);
//                        if (index != std::string::npos) {
//                            _document->row(row);
//                            _caret->row(0);
//                            _document->column(static_cast<int>(index));
//                            update_virtual_position();
//                            break;
//                        }
//
//                        start_col = 0;
//                    }
                } else if (tokens[0] == "quit") {
                    context()->engine()->quit();
                }
            }
            context()->erase_blackboard("text-editor-cmd");
        }
    }

    void controller::update_virtual_position() {
//        _vrow = _document->row() + _caret->row();
//        _vcol = _document->column() + _caret->column();

        if (_caret->mode() != core::caret::mode::select) {
            if (_selection_start.first != -1 || _selection_start.second != -1
            || _selection_end.first != -1 || _selection_end.second != -1) {
                _selection_start = {-1, -1};
                _selection_end = {-1, -1};
            }
        }
    }

    void controller::on_init(SDL_Renderer* renderer) {
        _font = context()->engine()->find_font("topaz-8");

        _caret = new core::caret(context(), nullptr, ids::caret_blink_timer, "hex-editor-caret");
        _caret->initialize(0, 0, _page_width, _page_height);
        _caret->rect()
                .pos(_left, _top)
                .size(_font->width, _font->line_height);
//        _document = new document(
//                "/Users/jeff/Temp/mon2.asm",
//                _page_width,
//                _page_height);
//        _document->load();
    }

    void controller::on_draw(SDL_Renderer* renderer) {
        auto width = context()->engine()->display_width();

        auto& white = (*context()->palette())[ide::context::colors::text];
        auto& grey = (*context()->palette())[ide::context::colors::info_text];

        auto info_color = grey.to_sdl_color();
        auto text_color = white.to_sdl_color();

//        if (_document != nullptr) {
//            FC_DrawColor(
//                    _font,
//                    renderer,
//                    5,
//                    5,
//                    info_color,
//                    "file: %s",
//                    _document->filename().c_str());
//        }

//        FC_DrawColor(
//                _font,
//                renderer,
//                350,
//                5,
//                info_color,
//                "C:%.2d/%.2d R:%.2d/%.2d",
//                _document->column() + 1,
//                line::max_columns,
//                _document->row() + 1,
//                document::max_rows);
        FC_DrawColor(
                _font->glyph,
                renderer,
                width - 200,
                5,
                info_color,
                "C:%.2d R:%.2d",
                _caret->column() + 1,
                _caret->row() + 1);
        FC_DrawColor(
                _font->glyph,
                renderer,
                width - 300,
                5,
                info_color,
                _caret->mode() == core::caret::mode::overwrite ? "OVR" : "INS");

//        auto y = _top;
//        auto row_start = _document->row();
//        auto row_stop = row_start + _page_height;
//        for (auto row = row_start; row < row_stop; row++) {
//            FC_DrawColor(_font, renderer, 5, y, info_color, "%.4d", row + 1);
//
//            auto col_start = _document->column();
//            auto col_end = col_start + 67;
//
//            std::stringstream stream;
//            for (auto col = col_start; col < col_end; col++) {
//                auto value = _document->get(row, col);
//                if (value == 0)
//                    stream << " ";
//                else
//                    stream << value;
//            }
//
//            FC_DrawColor(_font, renderer, _left, y, text_color, stream.str().c_str());
//
//            if ((row >= _selection_start.first && row <= _selection_end.first)
//                || (row >= _selection_end.first && row <= _selection_start.first)) {
//                SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0x7f);
//                SDL_Rect rect = {
//                        _left,
//                        y,
//                        _font_width * (_page_width + 1),
//                        _font_height
//                };
//                SDL_RenderFillRect(renderer, &rect);
//            }
//
//            y += _font_height;
//        }

        _caret->draw(renderer);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    bool controller::on_process_event(const SDL_Event* e) {
        auto ctrl_pressed = (SDL_GetModState() & KMOD_CTRL) != 0;
        auto shift_pressed = (SDL_GetModState() & KMOD_SHIFT) != 0;

        if (e->type == SDL_TEXTINPUT) {
//            if (_caret->mode() == core::caret::mode::insert)
//                _document->shift_right(_vrow, _vcol);
//            _document->put(_vrow, _vcol, static_cast<uint8_t>(e->text.text[0]));
            caret_right();
        } else if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_BACKQUOTE: {
                    if (ctrl_pressed)
                        return transition_to("cmd");
                    return false;
                }
                case SDLK_RETURN: {
//                    if (_caret->mode() == core::caret::mode::insert)
//                        _document->insert_line(_vrow);
                    caret_down();
                    return true;
                }
                case SDLK_TAB: {
                    if (shift_pressed) {
                        if (_caret->column() == 0)
                            return true;
//                        auto spaces = document::spaces_to_prev_tabstop(_vcol, 4);
//                        caret_left(spaces);
//                        _document->shift_left(_vrow, _vcol, spaces);
                    } else {
//                        auto spaces = document::spaces_to_next_tabstop(_vcol, 4);
//                        _document->shift_right(_vrow, _vcol, spaces);
//                        for (auto col = _vcol; col < _vcol + spaces; col++)
//                            _document->put(_vrow, col, 0);
//                        caret_right(spaces);
                    }
                    return true;
                }
                case SDLK_DELETE:
//                    _document->shift_left(_vrow, _vcol);
                    return true;

                case SDLK_BACKSPACE:
//                    if (_caret->column() == 0) {
//                        caret_up();
//                        _document->delete_line(_vrow);
//                    } else {
//                        caret_left();
//                        _document->shift_left(_vrow, _vcol);
//                    }
                    return true;

                case SDLK_UP:
                    caret_up();

                    if (shift_pressed)
                        update_selection();
                    else
                        end_selection();

                    return true;

                case SDLK_DOWN:
                    if (shift_pressed)
                        update_selection();
                    else
                        end_selection();

                    caret_down();

                    return true;

                case SDLK_LEFT:
                    caret_left();

                    if (shift_pressed)
                        update_selection();
                    else
                        end_selection();

                    return true;

                case SDLK_RIGHT:
                    caret_right();

                    if (shift_pressed)
                        update_selection();
                    else
                        end_selection();

                    return true;

                case SDLK_HOME:
                    if (ctrl_pressed)
                        first_page();
                    else
                        caret_home();
                    return true;

                case SDLK_END:
                    if (ctrl_pressed)
                        last_page();
                    else
                        caret_end();
                    return true;

                case SDLK_PAGEDOWN:
                    page_down();
                    return true;

                case SDLK_PAGEUP:
                    page_up();
                    return true;

                case SDLK_INSERT:
                    if (_caret->mode() == core::caret::mode::insert)
                        _caret->overwrite();
                    else
                        _caret->insert();
                    return true;
            }
        }
        return false;
    }

}