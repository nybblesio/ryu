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
        _lines.clear();
        raise_document_changed();
    }

    void document::page_up() {
        _row -= _page_height;
        clamp_row();
        raise_document_changed();
    }

    void document::shift_up() {
        if (_lines.empty())
            return;
        _lines.erase(_lines.begin());
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
        if (_row > _rows) {
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

    void document::first_page() {
        _row = 0;
        raise_document_changed();
    }

    bool document::scroll_left() {
        auto last_column = _column;
        --_column;
        auto clamped = clamp_column(last_column);
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
        auto clamped = clamp_column(_column);
        raise_document_changed();
        return clamped;
    }

    uint32_t document::row() const {
        return _row;
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

    uint16_t document::column() const {
        return _column;
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

    uint8_t document::page_height() const {
        return _page_height;
    }

    std::string document::filename() const {
        return _path.filename().string();
    }

    bool document::column(uint16_t column) {
        _column = column;
        auto clamped = clamp_column(_column);
        raise_document_changed();
        return clamped;
    }

    line_t* document::line_at(uint32_t row) {
        if (_lines.empty())
            return nullptr;
        if (row < _lines.size())
            return &_lines[row];
        return nullptr;
    }

    void document::delete_line(uint32_t row) {
        if (row < _lines.size())
            _lines.erase(_lines.begin() + row);
    }

    attr_t document::default_attr() const {
        return _default_attr;
    }

    line_t* document::insert_line(uint32_t row) {
        if (row < _lines.size()) {
            _lines.insert(_lines.begin() + row, line_t(_default_attr));
            return &_lines[row];
        }

        auto missing_count = (row - _lines.size()) + 1;
        for (size_t i = 0; i < missing_count; i++)
            _lines.push_back(line_t(_default_attr));
        return &_lines[row];
    }

    void document::raise_document_changed() {
        if (_document_changed_callback != nullptr)
            _document_changed_callback();
    }

    void document::default_attr(attr_t value) {
        _default_attr = value;
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
        uint32_t row = 0;
        std::string line;
        while (std::getline(stream, line)) {
            uint16_t col = 0;
            for (auto c : line) {
                auto value = static_cast<unsigned char>(c);
                if (value == 9) {
                    col += (4 - (col % 4));
                    continue;
                }
                put(row, col, element_t {value, _default_attr});
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

        try {
            std::fstream file;
            file.open(target_path.string());
            save(result, file);
            file.close();
        } catch (std::exception& e) {
            result.add_message(
                    "D001",
                    fmt::format("unable to save document: {}", e.what()),
                    true);
        }

        return !result.is_failed();
    }

    uint16_t document::find_line_end(uint32_t row) {
        auto line = line_at(row);
        if (line == nullptr || line->elements.empty())
            return 0;
        for (size_t col = line->elements.size() - 1; col != 0; col--) {
            auto element = line->get(col);
            if (element->value != 0)
                return static_cast<uint16_t>(col + 1);
        }
        return 0;
    }

    bool document::is_line_empty(uint32_t row) {
        auto line = line_at(row);
        if (line == nullptr)
            return true;
        auto empty = true;
        for (const auto& element : line->elements) {
            if (element.value != 0) {
                empty = false;
                break;
            }
        }
        return empty;
    }

    bool document::clamp_column(uint16_t last_col) {
        if (_column > last_col) {
            _column = last_col;
            return true;
        }

        auto right = _columns - _page_width;
        if (_column > right) {
            _column = right;
            return true;
        }

        return false;
    }

    element_t* document::get(uint32_t row, uint16_t column) {
        auto line = line_at(row);
        if (line == nullptr)
            return nullptr;
        return line->get(column);
    }

    void document::split_line(uint32_t row, uint16_t column) {
        insert_line(row + 1);
        auto old_line = line_at(row);
        if (old_line == nullptr)
            return;
        uint16_t col = 0;
        for (; column < _columns; column++) {
            auto old_element = get(row, column);
            put(row + 1, col++, old_element != nullptr ? *old_element : element_t {0, _default_attr});
            put(row, column, element_t { 0, _default_attr });
        }
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
        raise_document_changed();
    }

    void document::shift_left(uint32_t row, uint16_t column, uint16_t times) {
        auto line = line_at(row);
        if (line == nullptr)
            return;
        for (size_t i = 0; i < times && column < line->elements.size(); i++) {
            if (line->elements.empty())
                break;
            line->elements.erase(line->elements.begin() + column);
            line->elements.push_back(element_t{0, _default_attr});
        }
    }

    void document::shift_right(uint32_t row, uint16_t column, uint16_t times) {
        auto line = line_at(row);
        if (line == nullptr)
            return;
        for (size_t i = 0; i < times; i++, column++) {
            line->elements.insert(line->elements.begin() + column, element_t {0, _default_attr});
        }
    }

    void document::shift_line_left(uint32_t row, uint16_t column, uint16_t times) {
        auto line = line_at(row);
        if (line == nullptr)
            return;
        for (size_t i = 0; i < times && column < line->elements.size(); i++, column--) {
            line->elements.erase(line->elements.begin() + column);
        }
    }

    void document::put(uint32_t row, uint16_t column, const element_t& value) {
        auto line = line_at(row);
        if (line == nullptr) {
            line = insert_line(row);
        }
        line->put(column, value);
    }

    void document::shift_line_right(uint32_t row, uint16_t column, uint16_t times) {
        auto line = line_at(row);
        if (line == nullptr)
            return;
        for (size_t i = 0; i < times && column < _columns && column < line->elements.size(); i++, column++) {
            line->elements.insert(line->elements.begin() + column, element_t {0, _default_attr});
        }
    }

    void document::on_document_changed(const document::document_changed_callable& callable) {
        _document_changed_callback = callable;
    }

    attr_chunks document::get_line_chunks(uint32_t row, uint16_t column, uint16_t end_column) {
        auto line = line_at(row);
        if (line == nullptr)
            return {};

        uint16_t col = column;
        attr_chunks chunks;
        std::stringstream stream;
        auto element = line->get(col);
        if (element == nullptr)
            return {};

        auto last_attr = element->attr;
        while (col < end_column && col < line->elements.size()) {
            element = line->get(col);
            if (element == nullptr)
                break;
            if (last_attr != element->attr) {
                auto text = stream.str();
                if (!text.empty()) {
                    chunks.push_back(attr_chunk_t{last_attr, text});
                    stream.str(std::string());
                    stream.clear();
                }
                last_attr = element->attr;
            }
            if (element->value == 0)
                stream << " ";
            else if (element->value == 37)
                stream << "%%";
            else
                stream << element->value;
            ++col;
        }

        chunks.push_back(attr_chunk_t{last_attr, stream.str()});

        return chunks;
    }

    void document::write_line(std::ostream& stream, uint32_t row, uint16_t column, uint16_t end_column) {
        auto line = line_at(row);
        if (line == nullptr)
            return;
        for (uint16_t col = column; col < end_column && col < line->elements.size(); col++) {
            auto element = line->get(col);
            if (element == nullptr)
                break;
            if (element->value == 0)
                stream << " ";
            else if (element->value == 37)
                stream << "%%";
            else
                stream << element->value;
        }
    }

}