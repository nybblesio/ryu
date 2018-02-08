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
#include <core/views/caret.h>
#include "result.h"

namespace ryu::core {

    namespace fs = boost::filesystem;

    // Assumptions
    //
    // 1. We're building a text editor for Ryu, not anything else
    // 2. We're 99% editing assembly source code - or - source-like text
    // 3. We want to support colors, font styles, reverse video, and *maybe* links
    // 4. Documents in Ryu should rarely exceed 256kb
    // 5. Support document level marks
    //
    // 1. Buffers
    //
    //   read-only                          append-only
    // original buffer                  add/change buffer
    // ---------------                  ----------------
    // I <bold>love<> kittens.          <italic>furry<>
    //
    //
    // 2. Piece Table
    //
    // - Owns the buffers or reference/pointer to buffers
    // - Owns doubly-linked node-list (a/k/a piece)
    //
    // What a piece?
    //
    // struct piece_t {
    //      piece_t* prev;
    //      piece_t* next;
    //      buffer_t* buffer;
    //      uint32_t start;
    //      uint32_t length;
    // }
    //
    // When we start, the piece table has one entry:
    //
    // {nullptr, nullptr, original_ptr, 0, 15}
    //
    // User edits the document:  inserts "furry" after "love"
    //
    // piece_1_ptr {nullptr,     piece_2_ptr, original_ptr, 0, 6}
    // piece_2_ptr {piece_1_ptr, piece_3_ptr, append_ptr,   0, 5}
    // piece_3_ptr {piece_2_ptr, nullptr,     original_ptr, 6, 9}
    //
    //
    // Questions:
    //
    // - how do we render just one line?
    // - what do the buffers really look like?
    //

    struct document_position_t {
        uint16_t row {};
        uint8_t column {};
        uint32_t row_count {};
        uint16_t column_count {};
        uint32_t offset() const {
            return (row_count * column_count) + column;
        }
    };

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

    struct attr_span_t {
        attr_t attr;
        std::string text {};
    };

    typedef std::vector<attr_span_t> attr_span_list;

    struct element_t {
        attr_t attr {};
        uint8_t value = 0;

        inline bool is_tab() const {
            return value == '\t';
        }

        inline bool is_space() const {
            return isspace(value);
        }

        inline bool is_newline() const {
            return value == '\n';
        }

        inline bool is_percent() const {
            return value == 37;
        }

        inline bool safe_value(std::stringstream& stream) const {
            auto newline = false;
            if (is_space())
                stream << " ";
            else if (is_newline())
                newline = true;
            else if (is_percent())
                stream << "%%";
            else
                stream << value;
            return newline;
        }
    };

    struct piece_table_buffer_t;

    struct piece_t {
        attr_t attr {};
        uint32_t start {};
        size_t length {};
        piece_table_buffer_t* buffer = nullptr;

        inline size_t end() const {
            return start + length;
        }

        void copy_elements(attr_span_list& line);
    };

    struct piece_find_result_t {
        enum types {
            none,
            first,
            final,
            medial
        };
        bool at_back = false;
        piece_t* data = nullptr;
        types type = types::none;
        std::list<piece_t>::iterator index;
    };

    struct piece_list_t {
        void clear() {
            data.clear();
        }

        size_t total_length() const {
            size_t length = 0;
            for (auto& piece : data)
                length += piece.length;
            return length;
        }

        piece_find_result_t find_for_offset(uint32_t offset) {
            piece_find_result_t result {};
            if (data.empty()) {
                return result;
            }

            if (offset == 0) {
                result.data = &data.front();
                result.type = piece_find_result_t::types::first;
            } else if (offset == total_length()) {
                result.data = &data.back();
                result.type = piece_find_result_t::types::final;
            } else {
                auto piece_it = data.begin();
                while (piece_it != data.end()) {
                    auto& current_piece = *piece_it;
                    if (offset >= current_piece.start
                    &&  offset <= current_piece.end()) {
                        break;
                    }
                    ++piece_it;
                }

                if (piece_it != data.end()) {
                    result.data = &(*piece_it);
                    result.type = piece_find_result_t::types::medial;

                    ++piece_it;

                    result.index = piece_it;
                    result.at_back = piece_it == data.end();
                }
            }

            return result;
        }

        std::list<piece_t> data {};
    };

    struct piece_table_buffer_t {
        std::vector<element_t> elements {};

        void clear() {
            elements.clear();
        }
    };

    struct piece_table_t {
        void clear() {
            pieces.clear();
            changes.clear();
            original.clear();
            default_attr = {};
        }

        void delete_at(
                const document_position_t& position,
                size_t length) {
            if (length == 0)
                return;
        }

        //
        //
        // original: the brown fox
        //
        // change 1:               jumped
        // change 2:     sly[ ]
        // change 3:                      over the fence.
        // change 4: [ ]test:[ ]
        // change 5:       oooo
        //
        void insert(
                const element_t& element,
                const document_position_t& position) {
            changes.elements.push_back(element);

            auto offset = position.offset();
            auto find_result = pieces.find_for_offset(offset);
            auto is_change_piece = find_result.data != nullptr ?
                                   find_result.data->buffer == &changes :
                                   false;
            switch (find_result.type) {
                case piece_find_result_t::first: {
                    if (is_change_piece && find_result.data->end() + 1 == offset) {
                        find_result.data->length++;
                    } else {
                        pieces.data.push_back(piece_t {element.attr, offset, 1, &changes});
                    }
                    break;
                }
                case piece_find_result_t::final: {
                    if (is_change_piece) {
                        find_result.data->length++;
                    } else {
                        pieces.data.push_back(piece_t {element.attr, offset, 1, &changes});
                    }
                    break;
                }
                case piece_find_result_t::medial: {
                    //
                    //
                    // piece: 0................10
                    //           ^
                    //           3 index zero based
                    //   left: 0..2
                    //    new: 3
                    //  right: 4..95

                    auto new_piece = piece_t {element.attr, offset, 1, &changes};

                    auto& left_piece = find_result.data;
                    auto original_left_length = left_piece->length;

                    left_piece->length = offset - 1;

                    auto right_piece = piece_t {
                            left_piece->attr,
                            static_cast<uint32_t>(new_piece.end()),
                            original_left_length - (new_piece.length + left_piece->length),
                            left_piece->buffer
                    };

                    if (find_result.at_back) {
                        pieces.data.push_back(new_piece);
                        pieces.data.push_back(right_piece);
                    } else {
                        pieces.data.insert(find_result.index, right_piece);
                        pieces.data.insert(find_result.index, new_piece);
                    }
                    break;
                }
                default:
                    // XXX: should never happen
                    break;
            }
        }

        attr_span_list sequence() {
            attr_span_list spans {};
            for (auto& piece : pieces.data) {
                piece.copy_elements(spans);
            }
            return spans;
        }

        void load(const piece_table_buffer_t& buffer) {
            clear();
            original = buffer;

            // XXX: how do we restore the original styles
            pieces.data.push_back({default_attr, 0, original.elements.size(), &original});
        }

        piece_list_t pieces {};
        attr_t default_attr {};
        piece_table_buffer_t changes {};
        piece_table_buffer_t original {};
    };

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

        element_t* get();

        void first_page();

        void split_line();

        bool scroll_down();

        bool scroll_left();

        void delete_line();

        void insert_line();

        bool scroll_right();

        int32_t row() const;

        core::caret* caret();

        bool is_line_empty();

        fs::path path() const;

        uint32_t rows() const;

        bool row(uint32_t row);

        int16_t column() const;

        uint16_t find_line_end();

        uint16_t columns() const;

        void end(uint16_t column);

        uint8_t page_width() const;

        attr_t default_attr() const;

        uint8_t page_height() const;

        bool column(uint16_t column);

        void caret(core::caret* value);

        void default_attr(attr_t value);

        void path(const fs::path& value);

        void put(const element_t& value);

        attr_span_list line_at(uint32_t row);

        void shift_left(uint16_t times = 1);

        void shift_right(uint16_t times = 1);

        void shift_line_left(uint16_t times = 1);

        void shift_line_right(uint16_t times = 1);

        void page_size(uint8_t height, uint8_t width);

        void document_size(uint32_t rows, uint16_t columns);

        bool load(core::result& result, const fs::path& path);

        bool load(core::result& result, std::istream& stream);

        bool save(core::result& result, std::ostream& stream);

        bool save(core::result& result, const fs::path& path = "");

        void on_document_changed(const document_changed_callable& callable);

    protected:
        bool clamp_row();

        bool clamp_column();

        void raise_document_changed();

    private:
        fs::path _path;
        int32_t _row = 0;
        uint32_t _rows = 1;
        int16_t _column = 0;
        uint16_t _columns = 80;
        uint8_t _page_width = 0;
        uint8_t _page_height = 0;
        piece_table_t _piece_table {};
        core::caret* _caret = nullptr;
        document_changed_callable _document_changed_callback;
    };

}