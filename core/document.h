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
        explicit line_t(int columns, const attr_t& attr) {
            for (auto i = 0; i < columns; i++)
                elements.push_back(element_t{0, attr});
        }
        std::vector<element_t> elements;
    };

    struct attr_chunk_t {
        attr_t attr;
        std::string text {};
    };

    typedef std::vector<attr_chunk_t> attr_chunks;

    class document {
    public:
        static int spaces_to_prev_tabstop(int column, int tabsize);

        static int spaces_to_next_tabstop(int column, int tabsize);

        document() = default;

        ~document() = default;

        void clear();

        void shift_up();

        inline void home() {
            _column = 0;
        }

        inline void page_up() {
            _row -= _page_height;
            clamp_row();
        }

        inline int row() const {
            return _row;
        }

        inline bool scroll_up() {
            --_row;
            return clamp_row();
        }

        inline int rows() const {
            return _rows;
        }

        inline void page_down() {
            _row += _page_height;
            clamp_row();
        }

        void delete_line(int row);

        inline void last_page() {
            _row = _rows - _page_height;
            clamp_row();
        }

        bool initialized() const {
            return _initialized;
        }

        inline bool row(int row) {
            _row = row;
            return clamp_row();
        }

        inline void first_page() {
            _row = 0;
        }

        int find_line_end(int row);

        inline int column() const {
            return _column;
        }

        inline bool scroll_down() {
            ++_row;
            return clamp_row();
        }

        inline bool scroll_left() {
            --_column;
            return clamp_column();
        }

        line_t* insert_line(int row);

        inline int columns() const {
            return _columns;
        }

        inline bool scroll_right() {
            ++_column;
            return clamp_column();
        }

        bool is_line_empty(int row);

        inline void end(int column) {
            _column = _columns - column;
        }

        std::string filename() const {
            return _path.filename().string();
        }

        attr_t default_attr() const;

        inline bool column(int column) {
            _column = column;
            return clamp_column();
        }

        void load(const fs::path& path);

        void load(std::istream& stream);

        void save(std::ostream& stream);

        void default_attr(attr_t value);

        element_t* get(int row, int column);

        void split_line(int row, int column);

        void save(const fs::path& path = "");

        void page_size(int height, int width);

        void initialize(int rows, int columns);

        void shift_left(int row, int column, int times = 1);

        void shift_right(int row, int column, int times = 1);

        void put(int row, int column, const element_t& value);

        void shift_line_left(int row, int column, int times = 1);

        void shift_line_right(int row, int column, int times = 1);

        attr_chunks get_line_chunks(int row, int column, int end_column);

        void write_line(std::ostream& stream, int row, int column, int end_column);

    private:
        line_t* line_at(int row);

        bool clamp_column() {
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

        bool clamp_row() {
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

    private:
        int _rows;
        int _columns;
        int _row = 0;
        fs::path _path;
        int _column = 0;
        int _page_width = 0;
        int _page_height = 0;
        attr_t _default_attr {};
        bool _initialized = false;
        std::vector<line_t> _lines;
    };

}