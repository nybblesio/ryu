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
#include <common/stream_support.h>
#include "document.h"

namespace ryu::core {

    void document::home() {
        _column = 0;
        raise_document_changed();
    }

    void document::clear() {
        _row = 0;
        _column = 0;
        _lines.clear();
        raise_document_changed();
    }

    void document::select() {

    }

    void document::write_line(
            std::ostream& stream,
            uint32_t row,
            uint16_t start_column,
            uint16_t end_column) {
        auto line = line_at(row);
        if (line == nullptr)
            return;
        line->copy(stream, start_column, end_column);
    }

    void document::page_up() {
        _row -= _page_height;
        clamp_row();
        raise_document_changed();
    }

    void document::shift_up() {
        _lines.delete_at(0);
    }

    void document::line_end() {
        _caret->last_column();
        end(_page_width);
        while (true) {
            auto element = get();
            if (element != nullptr && element->value != 0) {
                _caret->right(1);
                break;
            }
            if (virtual_column() == 0)
                break;
            if (_caret->left(1))
                scroll_left();
        }
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

    element_t* document::get() {
        auto line = line_at();
        if (line == nullptr)
            return nullptr;
        return line->get(virtual_column());
    }

    void document::first_page() {
        _row = 0;
        raise_document_changed();
    }

    line_t* document::line_at() {
        return _lines.at(virtual_row());
    }

    void document::split_line() {
        _lines.split_at(virtual_row(), virtual_column());
    }

    bool document::scroll_left() {
        --_column;
        auto clamped = clamp_column();
        raise_document_changed();
        return clamped;
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

    void document::delete_line() {
        _lines.delete_at(virtual_row());
        raise_document_changed();
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

    bool document::is_line_empty() {
        auto line = line_at();
        if (line == nullptr)
            return true;
        return line->empty();
    }

    uint32_t document::rows() const {
        return _rows;
    }

    fs::path document::path() const {
        return _path;
    }

    line_t* document::insert_line() {
        auto line = _lines.insert_at(virtual_row());
        raise_document_changed();
        return line;
    }

    attr_t& document::default_attr() {
        return _lines.default_attr();
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

    void document::clear_selection() {
        auto line = line_at();
        if (line == nullptr)
            return;
        line->clear_selection();
        raise_document_changed();
    }

    uint16_t document::columns() const {
        return _columns;
    }

    uint16_t document::find_line_end() {
        auto line = line_at();
        if (line == nullptr)
            return 0;
        return line->end();
    }

    void document::end(uint16_t column) {
        _column = _columns - column;
        raise_document_changed();
    }

    attr_chunks document::get_line_chunks(
            uint32_t row,
            uint16_t start_column,
            uint16_t end_column) {
        auto line = line_at(row);
        if (line == nullptr)
            return {};
        return line->chunks(start_column, end_column);
    }

    core::caret* document::caret() const {
        return _caret;
    }

    uint8_t document::page_width() const {
        return _page_width;
    }

    uint8_t document::page_height() const {
        return _page_height;
    }

    uint32_t document::virtual_row() const {
        return static_cast<uint32_t>(_row + _caret->row());
    }

    bool document::column(uint16_t column) {
        _column = column;
        auto clamped = clamp_column();
        raise_document_changed();
        return clamped;
    }

    line_t* document::line_at(uint32_t row) {
        return _lines.at(row);
    }

    void document::raise_document_changed() {
        if (_document_changed_callback != nullptr)
            _document_changed_callback();
    }

    void document::caret(core::caret* value) {
        _caret = value;
    }

    uint16_t document::virtual_column() const {
        return static_cast<uint16_t>(_column + _caret->column());
    }

    void document::put(const element_t& value) {
        auto line = line_at();
        if (line == nullptr) {
            line = insert_line();
        }
        line->put(virtual_column(), value);
        raise_document_changed();
    }

    void document::path(const fs::path& value) {
        _path = value;
    }

    void document::shift_line_left(uint16_t times) {
        auto line = line_at();
        if (line == nullptr)
            return;
        line->shift_left(virtual_column(), times);
        raise_document_changed();
    }

    void document::shift_line_right(uint16_t times) {
        auto line = line_at();
        if (line == nullptr)
            return;
        line->shift_right(virtual_column(), times);
        raise_document_changed();
    }

    void document::page_size(uint8_t height, uint8_t width) {
        _page_width = width;
        if (_columns < _page_width)
            _columns = _page_width;

        _page_height = height;
        if (_rows < _page_height)
            _rows = _page_height;

        raise_document_changed();
    }

    void document::document_size(uint32_t rows, uint16_t columns) {
        _rows = rows;
        _columns = columns;
        _lines.line_width(_columns);
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

        std::stringstream stream;
        if (ryu::read_text(result, _path, stream)) {
            load(result, stream);
        }

        return !result.is_failed();
    }

    bool document::load(core::result& result, std::istream& stream) {
        clear();
        uint32_t row = 0;
        std::string line;
        while (std::getline(stream, line)) {
            uint16_t col = 0;
            auto doc_line = _lines.at(row);
            for (auto c : line) {
                auto value = static_cast<unsigned char>(c);
                if (value == 9) {
                    col += (4 - (col % 4));
                    continue;
                }
                doc_line->put(
                    col,
                    element_t {value, _lines.default_attr()});
                ++col;
            }
            ++row;
        }
        raise_document_changed();
        return true;
    }

    bool document::save(core::result& result, std::ostream& stream) {
        for (uint32_t row = 0; row < _lines.size(); row++) {
            std::stringstream line;
            write_line(line, row, 0, _columns);
            stream << ryu::rtrimmed(line.str()) << "\n";
        }
        raise_document_changed();
        return true;
    }

    bool document::save(core::result& result, const fs::path& path) {
        auto target_path = path;
        if (target_path.empty())
            target_path = _path;

        std::stringstream stream;
        save(result, stream);
        ryu::write_text(result, target_path, stream);

        return !result.is_failed();
    }

    void document::on_document_changed(const document::document_changed_callable& callable) {
        _document_changed_callback = callable;
    }

}