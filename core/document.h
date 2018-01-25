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
#include <boost/filesystem.hpp>
#include "result.h"

namespace ryu::core {

    namespace fs = boost::filesystem;

    struct attr_t {
        uint8_t color = 0;
        uint8_t style = 0;
        uint8_t flags = 0;
        bool operator== (const attr_t& rhs) const {
            return color == rhs.color && style == rhs.style && flags == rhs.flags;
        }
        bool operator!= (const attr_t& rhs) const {
            return color != rhs.color || style != rhs.style || flags != rhs.flags;
        }
    };

    struct element_t {
        uint8_t value = 0;
        attr_t attr;
    };

    struct line_t {
        explicit line_t(const attr_t& attr) : default_attr(attr) {
        }
        element_t* get(size_t column) {
            if (column < elements.size())
                return &elements[column];
            return nullptr;
        }
        void put(size_t column, const element_t& value) {
            if (column >= elements.size()) {
                auto missing_columns = (column - elements.size()) + 1;
                for (size_t i = 0; i < missing_columns; i++)
                    elements.push_back(element_t {0, default_attr});
            }
            elements[column] = value;
        }
        attr_t default_attr;
        std::vector<element_t> elements;
    };

    struct attr_chunk_t {
        attr_t attr;
        std::string text {};
    };

    typedef std::vector<attr_chunk_t> attr_chunks;

    class document {
    public:
        using document_changed_callable = std::function<void ()>;

        document() = default;

        ~document() = default;

        void home();

        void clear();

        void page_up();

        void shift_up();

        bool scroll_up();

        void page_down();

        void last_page();

        void first_page();

        bool scroll_down();

        bool scroll_left();

        bool scroll_right();

        uint32_t row() const;

        uint32_t rows() const;

        bool row(uint32_t row);

        uint16_t column() const;

        uint16_t columns() const;

        void end(uint16_t column);

        uint8_t page_width() const;

        attr_t default_attr() const;

        uint8_t page_height() const;

        bool column(uint16_t column);

        std::string filename() const;

        void delete_line(uint32_t row);

        void default_attr(attr_t value);

        bool is_line_empty(uint32_t row);

        line_t* insert_line(uint32_t row);

        inline size_t line_count() const {
            return _lines.size();
        }

        uint16_t find_line_end(uint32_t row);

        void page_size(uint8_t height, uint8_t width);

        element_t* get(uint32_t row, uint16_t column);

        void split_line(uint32_t row, uint16_t column);

        void document_size(uint32_t rows, uint16_t columns);

        bool load(core::result& result, const fs::path& path);

        bool load(core::result& result, std::istream& stream);

        bool save(core::result& result, std::ostream& stream);

        bool save(core::result& result, const fs::path& path = "");

        void put(uint32_t row, uint16_t column, const element_t& value);

        void shift_left(uint32_t row, uint16_t column, uint16_t times = 1);

        void on_document_changed(const document_changed_callable& callable);

        void shift_right(uint32_t row, uint16_t column, uint16_t times = 1);

        void shift_line_left(uint32_t row, uint16_t column, uint16_t times = 1);

        void shift_line_right(uint32_t row, uint16_t column, uint16_t times = 1);

        attr_chunks get_line_chunks(uint32_t row, uint16_t column, uint16_t end_column);

        void write_line(std::ostream& stream, uint32_t row, uint16_t column, uint16_t end_column);

    protected:
        bool clamp_row();

        void raise_document_changed();

        line_t* line_at(uint32_t row);

        bool clamp_column(uint16_t last_col);

    private:
        fs::path _path;
        uint32_t _row = 0;
        uint32_t _rows = 1;
        uint16_t _column = 0;
        uint16_t _columns = 80;
        attr_t _default_attr {};
        uint8_t _page_width = 0;
        uint8_t _page_height = 0;
        std::vector<line_t> _lines;
        document_changed_callable _document_changed_callback;
    };

}