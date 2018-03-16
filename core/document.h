//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#pragma once

#include <map>
#include <list>
#include <core/views/caret.h>
#include <boost/filesystem.hpp>
#include "result.h"

namespace ryu::core {

    namespace fs = boost::filesystem;

    struct element_t {
        uint8_t value = 0;
        attr_t attr;
    };

    class line_t {
    public:
        line_t(
                uint16_t width,
                const attr_t& attr) : _default_attr(attr) {
            _elements.resize(width);
        }

        void copy(
                std::ostream& stream,
                uint16_t start_column,
                uint16_t end_column) {
            for (uint16_t col = start_column;
                 col < end_column && col < _elements.size();
                 col++) {
                const auto& element = _elements[col];
                if (element.value == 0)
                    stream << " ";
                else if (element.value == 37)
                    stream << "%%";
                else
                    stream << element.value;
            }
        }

        bool empty() const {
            if (_elements.empty())
                return true;
            auto is_empty = true;
            for (const auto& element : _elements) {
                if (element.value != 0) {
                    is_empty = false;
                    break;
                }
            }
            return is_empty;
        }

        uint16_t end() const {
            for (size_t col = _elements.size() - 1; col != 0; col--) {
                const auto& element = _elements[col];
                if (element.value != 0)
                    return static_cast<uint16_t>(col + 1);
            }
            return 0;
        }

        size_t length() const {
            return _elements.size();
        }

        void clear_selection() {
            _selection_end = 0;
            _selection_start = 0;
        }

        bool has_selection() const {
            return _selection_end - _selection_start > 0;
        }

        element_t* get(size_t column) {
            if (column < _elements.size())
                return &_elements[column];
            return nullptr;
        }

        uint16_t selection_end() const {
            return _selection_end;
        }

        uint16_t selection_start() const {
            return _selection_start;
        }

        void selection_end(uint16_t value) {
            _selection_end = value;
        }

        void selection_start(uint16_t value) {
            _selection_start = value;
        }

        void put(size_t column, const element_t& value) {
            if (column >= _elements.size())
                add_missing_elements((column + 1) - _elements.size());
            _elements[column] = value;
        }

        void shift_left(uint16_t column, uint16_t times) {
            for (size_t i = 0;
                 i < times && column < _elements.size();
                 i++, column--) {
                _elements.erase(_elements.begin() + column);
                _elements.push_back(element_t {0, _default_attr});
            }
        }

        void shift_right(uint16_t column, uint16_t times) {
            for (size_t i = 0;
                 i < times && column < _elements.size();
                 i++, column++) {
                _elements.insert(_elements.begin() + column, element_t {0, _default_attr});
            }
        }

        void apply_selection(
                attr_t& attr,
                uint16_t start_column,
                uint16_t end_column) {
            if (!has_selection())
                return;
            if (_selection_start >= start_column && _selection_end <= end_column)
                attr.flags |= core::font::flags::reverse;
        }

        attr_chunks chunks(uint16_t start_column, uint16_t end_column) {
            if (_elements.empty())
                return {};

            attr_chunks chunks {};
            uint16_t col = start_column;
            std::stringstream stream;

            auto last_attr = _elements[col].attr;
            apply_selection(last_attr, start_column, end_column);

            while (col < end_column && col < _elements.size()) {
                const auto& element = _elements[col];
                if (last_attr != element.attr) {
                    auto text = stream.str();
                    if (!text.empty()) {
                        chunks.push_back(attr_chunk_t{last_attr, text});
                        stream.str(std::string());
                        stream.clear();
                    }
                    last_attr = element.attr;
                    apply_selection(last_attr, start_column, end_column);
                }
                if (element.value == 0)
                    stream << " ";
                else if (element.value == 37)
                    stream << "%%";
                else
                    stream << element.value;
                ++col;
            }

            chunks.push_back(attr_chunk_t{
                last_attr,
                stream.str()});

            return chunks;
        }

    private:
        void add_missing_elements(size_t count) {
            for (size_t i = 0; i < count; i++)
                _elements.push_back(element_t {0, _default_attr});
        }

    private:
        attr_t _default_attr;
        uint16_t _selection_end;
        uint16_t _selection_start;
        std::vector<element_t> _elements {};
    };

    class line_list {
    public:
        void clear() {
            _lines.clear();
        }

        bool empty() const {
            return _lines.empty();
        }

        size_t size() const {
            return _lines.size();
        }

        attr_t& default_attr() {
            return _default_attr;
        }

        line_t* at(uint32_t row) {
            if (_lines.empty() || row > _lines.size() - 1)
                add_missing_lines((row + 1) - _lines.size());
            return &_lines[row];
        }

        void delete_at(uint32_t row) {
            if (row < _lines.size())
                _lines.erase(_lines.begin() + row);
        }

        void line_width(uint16_t width) {
            _width = width;
        }

        line_t* insert_at(uint32_t row) {
            if (_lines.empty() || row > _lines.size() - 1) {
                add_missing_lines((row + 1) - _lines.size());
            } else {
                _lines.insert(
                    _lines.begin() + row,
                    line_t {_width, _default_attr});
            }
            return &_lines[row];
        }

        void default_attr(const attr_t& attr) {
            _default_attr = attr;
        }

        void split_at(uint32_t row, uint32_t column) {
            auto current_line = at(row);
            auto new_line = insert_at(row);
            uint16_t col = 0;
            for (; column < current_line->length(); column++) {
                auto old_element = current_line->get(column);
                new_line->put(
                    col++,
                    old_element != nullptr ? *old_element : element_t {0, _default_attr});
                current_line->put(
                    column,
                    element_t {0, _default_attr});
            }
        }

    private:
        void add_missing_lines(size_t count) {
            for (size_t i = 0; i < count; i++)
                _lines.emplace_back(_width, _default_attr);
        }

    private:
        uint16_t _width = 0;
        attr_t _default_attr {};
        std::vector<line_t> _lines {};
    };

    class document {
    public:
        using document_changed_callable = std::function<void ()>;

        document() = default;

        ~document() = default;

        bool load(
            core::result& result,
            const fs::path& path);

        bool load(
            core::result& result,
            std::istream& stream);

        bool save(
            core::result& result,
            std::ostream& stream);

        bool save(
            core::result& result,
            const fs::path& path = "");

        void home();

        void clear();

        void select();

        void page_up();

        void page_size(
            uint8_t height,
            uint8_t width);

        void shift_up();

        element_t* get();

        void write_line(
            std::ostream& stream,
            uint32_t row,
            uint16_t start_column,
            uint16_t end_column);

        bool scroll_up();

        void page_down();

        void last_page();

        void first_page();

        void split_line();

        void delete_line();

        bool scroll_down();

        bool scroll_left();

        bool scroll_right();

        int32_t row() const;

        void document_size(
            uint32_t rows,
            uint16_t columns);

        bool is_line_empty();

        line_t* insert_line();

        fs::path path() const;

        uint32_t rows() const;

        bool row(uint32_t row);

        int16_t column() const;

        attr_t& default_attr();

        void clear_selection();

        uint16_t find_line_end();

        uint16_t columns() const;

        void end(uint16_t column);

        uint8_t page_width() const;

        core::caret* caret() const;

        uint8_t page_height() const;

        bool column(uint16_t column);

        uint32_t virtual_row() const;

        attr_chunks get_line_chunks(
            uint32_t row,
            uint16_t start_column,
            uint16_t end_column);

        void caret(core::caret* value);

        uint16_t virtual_column() const;

        void put(const element_t& value);

        void path(const fs::path& value);

        inline size_t line_count() const {
            return _lines.size();
        }

        void shift_line_left(uint16_t times = 1);

        void shift_line_right(uint16_t times = 1);

        void on_document_changed(const document_changed_callable& callable);

    protected:
        bool clamp_row();

        line_t* line_at();

        bool clamp_column();

        void raise_document_changed();

        line_t* line_at(uint32_t row);

    private:
        fs::path _path;
        int32_t _row = 0;
        uint32_t _rows = 1;
        line_list _lines {};
        int16_t _column = 0;
        uint16_t _columns = 80;
        uint8_t _page_width = 0;
        uint8_t _page_height = 0;
        core::caret* _caret = nullptr;
        document_changed_callable _document_changed_callback;
    };

}