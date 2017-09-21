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

    int document::spaces_to_prev_tabstop(int column, int tabsize) {
        auto spaces = 0;
        if (column % tabsize == 0)
            column--;
        for (auto j = column; j > column - tabsize; j--) {
            if (j % tabsize == 0) break;
            spaces++;
        }
        return spaces;
    }

    int document::spaces_to_next_tabstop(int column, int tabsize) {
        auto spaces = 0;
        if (column % tabsize == 0)
            column++;
        for (auto j = column; j < column + tabsize; j++) {
            if (j % tabsize == 0) break;
            spaces++;
        }
        return spaces;
    }

    document::~document() {
        delete[] _data;
        _data = nullptr;

        delete[] _attrs;
        _attrs = nullptr;
    }

    void document::clear() {
        std::memset(_data, 0, _rows * _columns);
        std::memset(_attrs, 0, _rows * _columns);
    }

    void document::initialize(
            int rows,
            int columns,
            int page_width,
            int page_height) {
        _rows = rows;
        _columns = columns;
        _page_width = page_width;
        _page_height = page_height;
        _data = new uint8_t[_rows * _columns];
        _attrs = new uint8_t[_rows * _columns];
    }

    void document::shift_up() {
        auto last_row = (_rows - 1) * _columns;

        std::memcpy(_data, _data + _columns, static_cast<size_t>(last_row));
        std::memset(_data + last_row, 0, static_cast<size_t>(_columns));

        std::memcpy(_attrs, _attrs + _columns, static_cast<size_t>(last_row));
        std::memset(_attrs + last_row, 0, static_cast<size_t>(_columns));
    }

    uint8_t document::default_attr() const {
        return _default_attr;
    }

    void document::default_attr(uint8_t value) {
        _default_attr = value;
    }

    void document::load(const fs::path& path) {
        _path = path;
        std::fstream file;
        file.open(_path.string());
        load(file);
        file.close();
    }

    void document::load(std::istream& stream) {
        clear();
        auto row = 0;
        std::string line;
        while (std::getline(stream, line)) {
            auto col = 0;
            for (auto c : line) {
                auto value = static_cast<unsigned char>(c);
                if (value == 9) {
                    col += spaces_to_next_tabstop(col, 4);
                    continue;
                }
                put(row, col, value);
                put_attr(row, col, _default_attr);
                ++col;
            }
            ++row;
        }
    }

    void document::save(std::ostream& stream) {
        for (auto row = 0; row < _rows; row++) {
            std::stringstream line;
            write_line(line, row, 0, _columns);
            stream << ryu::rtrimmed(line.str()) << "\n";
        }
    }

    void document::save(const fs::path& path) {
        auto target_path = path;
        if (target_path.empty())
            target_path = _path;
        std::fstream file;
        file.open(target_path.string());
        save(file);
        file.close();
    }

    void document::delete_line(int row) {
        auto size = (_rows * _columns) - 1;
        auto row_offset = row * _columns;
        auto previous_row_offset = std::max((row - 1), 0) * _columns;

        std::memcpy(_data + previous_row_offset, _data + row_offset, static_cast<size_t>(size - row_offset));
        std::memset(_data + ((_rows - 1) * _columns), 0, static_cast<size_t>(_columns));

        std::memcpy(_attrs + previous_row_offset, _attrs + row_offset, static_cast<size_t>(size - row_offset));
        std::memset(_attrs + ((_rows - 1) * _columns), 0, static_cast<size_t>(_columns));
    }

    void document::insert_line(int row) {
        auto size = (_rows * _columns) - 1;
        auto row_offset = (row + 1) * _columns;
        auto next_row_offset = (row + 2) * _columns;

        std::memcpy(_data + next_row_offset, _data + row_offset, static_cast<size_t>(size - next_row_offset));
        std::memset(_data + row_offset, 0, static_cast<size_t>(_columns));

        std::memcpy(_attrs + next_row_offset, _attrs + row_offset, static_cast<size_t>(size - next_row_offset));
        std::memset(_attrs + row_offset, 0, static_cast<size_t>(_columns));
    }

    int document::find_line_end(int row) {
        auto last_empty = -1;
        for (auto col = 0; col < _columns; col++) {
            auto value = get(row, col);
            if (value == 0) {
                if (last_empty == -1)
                    last_empty = col;
            } else
                last_empty = -1;
        }
        return last_empty == -1 ? 0 : last_empty;
    }

    bool document::is_line_empty(int row) {
        auto empty = true;
        for (auto col = 0; col < _columns; col++) {
            auto value = get(row, col);
            if (value != 0) {
                empty = false;
                break;
            }
        }
        return empty;
    }

    uint8_t document::get(int row, int column) {
        return _data[(row * _columns) + column];
    }

    void document::split_line(int row, int column) {
        insert_line(row);
        auto offset = (row * _columns) + column;
        auto new_line_offset = (row + 1) * _columns;
        auto size = static_cast<size_t>(find_line_end(row) - column);
        std::memcpy(_data + new_line_offset, _data + offset, size);
        std::memcpy(_attrs + new_line_offset, _attrs + offset, size);
        std::memset(_data + offset, 0, size);
        std::memset(_attrs + offset, 0, size);
    }

    uint8_t document::get_attr(int row, int column) {
        return _attrs[(row * _columns) + column];
    }

    void document::shift_left(int row, int column, int times) {
        auto size = (_rows * _columns) - 1;
        for (auto i = 0; i < times; i++, column--) {
            auto offset = (row * _columns) + column;
            if (offset + 1 < size) {
                std::memcpy(_data + offset, _data + offset + 1, static_cast<size_t>(size - offset));
                std::memcpy(_attrs + offset, _attrs + offset + 1, static_cast<size_t>(size - offset));
            }
            else {
                std::memset(_data + offset, 0, 1);
                std::memset(_attrs + offset, 0, 1);
            }
        }
    }

    void document::shift_right(int row, int column, int times) {
        auto size = (_rows * _columns) - 1;
        for (auto i = 0; i < times; i++, column++) {
            auto offset = (row * _columns) + column;
            if (offset + 1 < size) {
                std::memcpy(_data + offset + 1, _data + offset, static_cast<size_t>(size - offset));
                std::memcpy(_attrs + offset + 1, _attrs + offset, static_cast<size_t>(size - offset));
            }
        }
    }

    void document::put(int row, int column, uint8_t value) {
        auto offset = (row * _columns) + column;
        _data[offset] = value;
    }

    void document::put_attr(int row, int column, uint8_t value) {
        _attrs[(row * _columns) + column] = value;
    }

    void document::shift_line_left(int row, int column, int times) {
        auto size = _columns - 1;
        for (auto i = 0; i < times; i++, column--) {
            auto offset = (row * _columns) + column;
            std::memcpy(_data + offset, _data + offset + 1, static_cast<size_t>(size - column));
            std::memcpy(_attrs + offset, _attrs + offset + 1, static_cast<size_t>(size - column));
        }
    }

    void document::shift_line_right(int row, int column, int times) {
        auto size = _columns - 1;
        for (auto i = 0; i < times && column < _columns; i++, column++) {
            auto offset = (row * _columns) + column;
            std::memcpy(_data + offset + 1, _data + offset, static_cast<size_t>(size - column));
            _data[offset] = 0;

            std::memcpy(_attrs + offset + 1, _attrs + offset, static_cast<size_t>(size - column));
            _attrs[offset] = 0;
        }
    }

    attr_chunks document::get_line_chunks(int line, int column, int end_column) {
        attr_chunks chunks;
        std::stringstream stream;

        uint8_t last_attr = 0;
        auto col = column;
        while (col < end_column) {
            auto attr = get_attr(line, col);
            if (attr != last_attr) {
                auto text = stream.str();
                if (!text.empty()) {
                    chunks.push_back(attr_chunk_t{last_attr, text});
                    stream.str(std::string());
                    stream.clear();
                }
                last_attr = attr;
            }
            auto value = get(line, col);
            if (value == 0)
                stream << " ";
            else if (value == 37)
                stream << "%%";
            else
                stream << value;
            ++col;
        }

        chunks.push_back(attr_chunk_t{last_attr, stream.str()});

        return chunks;
    }

    void document::write_line(std::ostream& stream, int line, int column, int end_column) {
        for (auto col = column; col < end_column; col++) {
            auto value = get(line, col);
            if (value == 0)
                stream << " ";
            else if (value == 37)
                stream << "%%";
            else
                stream << value;
        }
    }

}