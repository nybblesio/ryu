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

    void document::clear() {
        _lines.clear();
    }

    void document::shift_up() {
        if (_lines.empty())
            return;
        _lines.erase(_lines.begin());
    }

    line_t* document::line_at(int row) {
        if (_lines.empty())
            return nullptr;
        if (row < _lines.size())
            return &_lines[row];
        return nullptr;
    }

    void document::delete_line(int row) {
        auto previous_row = row - 1;
        if (previous_row < 0)
            previous_row = 0;
        if (previous_row < _lines.size())
            _lines.erase(_lines.begin() + previous_row);
    }

    attr_t document::default_attr() const {
        return _default_attr;
    }

    line_t* document::insert_line(int row) {
        if (row > _lines.size() - 1) {
            for (size_t i = 0; i < row; i++)
                _lines.push_back(line_t(_columns, _default_attr));
        } else {
            _lines.insert(_lines.begin() + row, line_t(_columns, _default_attr));
        }
        return &_lines[row];
    }

    void document::default_attr(attr_t value) {
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
                put(row, col, element_t {value, _default_attr});
                ++col;
            }
            ++row;
        }
    }

    void document::save(std::ostream& stream) {
        for (auto row = 0; row < _lines.size(); row++) {
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

    int document::find_line_end(int row) {
        auto line = line_at(row);
        if (line == nullptr)
            return 0;
        auto last_empty = -1;
        auto col = 0;
        for (auto element : line->elements) {
            if (element.value == 0) {
                if (last_empty == -1)
                    last_empty = col;
            } else
                last_empty = -1;
            col++;
        }
        return last_empty == -1 ? 0 : last_empty;
    }

    bool document::is_line_empty(int row) {
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

    element_t* document::get(int row, int column) {
        auto line = line_at(row);
        if (line == nullptr)
            return nullptr;
        if (column < line->elements.size())
            return &line->elements[column];
        return nullptr;
    }

    void document::split_line(int row, int column) {
        insert_line(row + 1);
        auto old_line = line_at(row);
        if (old_line == nullptr)
            return;
        auto col = 0;
        for (; column < _columns; column++) {
            auto old_element = get(row, column);
            put(row + 1, col++, old_element != nullptr ? *old_element : element_t {0, _default_attr});
            put(row, column, element_t { 0, _default_attr });
        }
    }

    void document::page_size(int height, int width) {
        _page_width = width;
        if (_columns < _page_width)
            _columns = _page_width;

        _page_height = height;
        if (_rows < _page_height)
            _rows = _page_height;

        for (auto& line : _lines) {
            if (line.elements.size() < _page_width) {
                auto missing_count = _page_width - line.elements.size();
                for (auto i = 0; i < missing_count; i++)
                    line.elements.push_back(element_t {0, _default_attr});
            }
        }
    }

    void document::initialize(int rows, int columns) {
        _rows = rows;
        _columns = columns;
        _initialized = true;
    }

    void document::shift_left(int row, int column, int times) {
        auto line = line_at(row);
        if (line == nullptr)
            return;
        for (auto i = 0; i < times && column < line->elements.size(); i++, column--) {
            if (line->elements.empty())
                break;
            line->elements.erase(line->elements.begin() + column);
            line->elements.push_back(element_t{0, _default_attr});
//            for (auto& below : _lines) {
//                if (below.row > line->row) {
//                    line->elements.erase(line->elements.begin());
//                }
//            }
        }
    }

    void document::shift_right(int row, int column, int times) {
        auto line = line_at(row);
        if (line == nullptr)
            return;
        for (auto i = 0; i < times; i++, column++) {
            line->elements.insert(line->elements.begin() + column, element_t {0, _default_attr});
//            for (auto& below : _lines) {
//                if (below.row > line->row) {
//                    line->elements.insert(line->elements.begin(), element_t {0, _default_attr});
//                }
//            }
        }
    }

    void document::shift_line_left(int row, int column, int times) {
        auto line = line_at(row);
        if (line == nullptr)
            return;
        for (auto i = 0; i < times && column < line->elements.size(); i++, column--) {
            line->elements.erase(line->elements.begin() + column);
        }
    }

    void document::put(int row, int column, const element_t& value) {
        auto line = line_at(row);
        if (line == nullptr) {
            line = insert_line(row);
        }
        if (column > line->elements.size()) {
            auto missing_count = column - line->elements.size();
            for (auto i = 0; i < missing_count; i++)
                line->elements.push_back(element_t {0, _default_attr});
        }
        line->elements[column] = value;
    }

    void document::shift_line_right(int row, int column, int times) {
        auto line = line_at(row);
        if (line == nullptr)
            return;
        for (auto i = 0; i < times && column < _columns && column < line->elements.size(); i++, column++) {
            line->elements.insert(line->elements.begin() + column, element_t {0, _default_attr});
        }
    }

    attr_chunks document::get_line_chunks(int row, int column, int end_column) {
        auto line = line_at(row);
        if (line == nullptr)
            return {};

        auto col = column;
        attr_chunks chunks;
        std::stringstream stream;
        attr_t last_attr = line->elements[col].attr;

        while (col < end_column && col < line->elements.size()) {
            auto& element = line->elements[col];
            if (last_attr != element.attr) {
                auto text = stream.str();
                if (!text.empty()) {
                    chunks.push_back(attr_chunk_t{last_attr, text});
                    stream.str(std::string());
                    stream.clear();
                }
                last_attr = element.attr;
            }
            if (element.value == 0)
                stream << " ";
            else if (element.value == 37)
                stream << "%%";
            else
                stream << element.value;
            ++col;
        }

        chunks.push_back(attr_chunk_t{last_attr, stream.str()});

        return chunks;
    }

    void document::write_line(std::ostream& stream, int row, int column, int end_column) {
        auto line = line_at(row);
        if (line == nullptr)
            return;
        for (auto col = column; col < end_column && col < line->elements.size(); col++) {
            const auto& element = line->elements[col];
            if (element.value == 0)
                stream << " ";
            else if (element.value == 37)
                stream << "%%";
            else
                stream << element.value;
        }
    }

}