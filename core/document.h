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

#include <list>
#include <boost/filesystem.hpp>
#include "core/state.h"

namespace ryu::core {

    namespace fs = boost::filesystem;

    struct attr_chunk_t {
        uint8_t attr = 0;
        std::string text {};
    };

    typedef std::vector<attr_chunk_t> attr_chunks;

    class document {
    public:
        static int spaces_to_prev_tabstop(int column, int tabsize);

        static int spaces_to_next_tabstop(int column, int tabsize);

        document() = default;

        ~document();

        void clear();

        void shift_up();

        void initialize(
                int rows,
                int columns,
                int page_width,
                int page_height);

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

        void insert_line(int row);

        void delete_line(int row);

        inline void last_page() {
            _row = _rows - _page_height;
            clamp_row();
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

        inline bool column(int column) {
            _column = column;
            return clamp_column();
        }

        void load(const fs::path& path);

        void load(std::istream& stream);

        void save(std::ostream& stream);

        uint8_t get(int row, int column);

        void save(const fs::path& path = "");

        uint8_t get_attr(int row, int column);

        void put(int row, int column, uint8_t value);

        void put_attr(int row, int column, uint8_t value);

        void shift_left(int row, int column, int times = 1);

        void shift_right(int row, int column, int times = 1);

        void shift_line_left(int row, int column, int times = 1);

        void shift_line_right(int row, int column, int times = 1);

        attr_chunks get_line_chunks(int line, int column, int end_column);

        void write_line(std::ostream& stream, int line, int column, int end_column);

    private:
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
        uint8_t* _data = nullptr;
        uint8_t* _attrs = nullptr;
    };

}