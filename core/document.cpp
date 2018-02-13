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
#include <common/string_support.h>
#include "document.h"

namespace ryu::core {

    void document::home() {
        _column = 0;
        raise_document_changed();
    }

    void document::clear() {
        _row = 0;
        _column = 0;
        _piece_table.load({});
        raise_document_changed();
    }

    void document::page_up() {
        _row -= _page_height;
        clamp_row();
        raise_document_changed();
    }

    void document::shift_up() {
    }

    void document::split_line() {
        if (_caret->mode() == caret::mode::types::insert) {

        }
    }

    void document::first_page() {
        _row = 0;
        raise_document_changed();
    }

    element_t* document::get() {
        return nullptr;
    }

    void document::page_down() {
        _row += _page_height;
        clamp_row();
        raise_document_changed();
    }

    void document::last_page() {
        _row = _rows - _page_height;
        clamp_row();
        raise_document_changed();
    }

    bool document::scroll_up() {
        --_row;
        auto clamped = clamp_row();
        raise_document_changed();
        return clamped;
    }

    bool document::clamp_row() {
        if (_row < 0) {
            _row = 0;
            return true;
        }

        auto bottom = _rows - _page_height;
        if (_row > bottom) {
            _row = bottom;
            return true;
        }

        return false;
    }

    void document::insert_line() {
    }

    void document::delete_line() {
    }

    bool document::scroll_left() {
        --_column;
        auto clamped = clamp_column();
        raise_document_changed();
        return clamped;
    }

    bool document::scroll_down() {
        ++_row;
        auto clamped = clamp_row();
        raise_document_changed();
        return clamped;
    }

    bool document::scroll_right() {
        ++_column;
        auto clamped = clamp_column();
        raise_document_changed();
        return clamped;
    }

    int32_t document::row() const {
        return _row;
    }

    bool document::clamp_column() {
        if (_column < 0) {
            _column = 0;
            return true;
        }

        auto right = _columns - _page_width;
        if (_column > right) {
            _column = right;
            return true;
        }

        return false;
    }

    bool document::is_line_empty() {
        return true;
    }

    core::caret* document::caret() {
        return _caret;
    }

    fs::path document::path() const {
        return _path;
    }

    uint32_t document::rows() const {
        return _rows;
    }

    bool document::row(uint32_t row) {
        _row = row;
        auto clamped = clamp_row();
        raise_document_changed();
        return clamped;
    }

    int16_t document::column() const {
        return _column;
    }

    uint16_t document::find_line_end() {
        return 0;
    }

    uint16_t document::columns() const {
        return _columns;
    }

    void document::end(uint16_t column) {
        _column = _columns - column;
        raise_document_changed();
    }

    uint8_t document::page_width() const {
        return _page_width;
    }

    attr_t document::default_attr() const {
        return _piece_table.default_attr;
    }

    uint8_t document::page_height() const {
        return _page_height;
    }

    bool document::column(uint16_t column) {
        _column = column;
        auto clamped = clamp_column();
        raise_document_changed();
        return clamped;
    }

    void document::raise_document_changed() {
        if (_document_changed_callback != nullptr)
            _document_changed_callback();
    }

    void document::default_attr(attr_t value) {
        _piece_table.default_attr = value;
    }

    void document::caret(core::caret* value) {
        _caret = value;
    }

    void document::shift_left(uint16_t times) {
    }

    void document::put(const element_t& value) {
        _piece_table.insert_at(
                static_cast<uint32_t>(((_row + _caret->row()) * _columns) + (_column + _caret->column())),
                value);
    }

    void document::shift_right(uint16_t times) {
    }

    void document::path(const fs::path& value) {
        _path = value;
    }

    attr_span_list document::line_at(uint32_t row) {
        return {};
    }

    void document::shift_line_left(uint16_t times) {
    }

    void document::shift_line_right(uint16_t times) {
        if (_caret->mode() == caret::mode::types::insert) {

        }
    }

    void document::page_size(uint8_t height, uint8_t width) {
        _page_width = width;
        _page_height = height;

        _rows = std::min<uint8_t>(_page_height, static_cast<uint8_t>(_rows));
        _columns = std::min<uint8_t>(_page_width, static_cast<uint8_t>(_columns));

        if (_caret != nullptr) {
            _caret->page_size(height, width);
        }

        raise_document_changed();
    }

    void document::document_size(uint32_t rows, uint16_t columns) {
        _rows = rows;
        _columns = columns;
        raise_document_changed();
    }

    bool document::load(core::result& result, const fs::path& path) {
        _path = path;

        if (!fs::exists(_path)) {
            result.add_message(
                    "D001",
                    fmt::format("document does not exist: {}", _path.string()),
                    true);
            return false;
        }

        try {
            std::fstream file;
            file.open(_path.string());
            load(result, file);
            file.close();
        } catch (std::exception& e) {
            result.add_message(
                    "D001",
                    fmt::format("unable to load document: {}", e.what()),
                    true);
        }

        return !result.is_failed();
    }

    bool document::load(core::result& result, std::istream& stream) {
        clear();

        raise_document_changed();
        return true;
    }

    bool document::save(core::result& result, std::ostream& stream) {
        raise_document_changed();
        return true;
    }

    bool document::save(core::result& result, const fs::path& path) {
        auto target_path = path;
        if (target_path.empty())
            target_path = _path;

        try {
            std::ofstream file(target_path.string());
            if (file.is_open()) {
                save(result, file);
                file.close();
            }
        } catch (std::exception& e) {
            result.add_message(
                    "D001",
                    fmt::format("unable to save document: {}", e.what()),
                    true);
        }

        return !result.is_failed();
    }

    void document::on_document_changed(const document::document_changed_callable& callable) {
        _document_changed_callback = callable;
    }

}