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

#include <set>
#include <list>
#include <string>
#include <vector>
#include <cstdint>

namespace ryu::core {

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
            return (row * column_count) + column;
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
        attr_t attr {};
        std::string text {};
    };

    typedef std::vector<attr_span_t> attr_span_list;
    typedef std::vector<attr_span_list> attr_line_list;

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

        bool safe_value(std::stringstream& stream) const;
    };

    struct piece_table_buffer_t;

    struct piece_t {
        uint32_t start {};
        size_t length {};
        piece_table_buffer_t* buffer = nullptr;

        inline size_t end() const {
            return start + length;
        }

        void copy_elements(attr_line_list& lines);

        bool operator== (const piece_t& other) {
            return start == other.start
                   && length == other.length
                   && buffer == other.buffer;
        }

        bool operator!= (const piece_t& other) {
            return !(*this == other);
        }

        bool operator== (const piece_t& other) const {
            return start == other.start
                   && length == other.length
                   && buffer == other.buffer;
        }

        bool operator!= (const piece_t& other) const {
            return !(*this == other);
        }
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
        std::list<piece_t>::iterator index {};
    };

    struct piece_list_t {
        void clear() {
            data.clear();
        }

        inline bool empty() const {
            return data.empty();
        }

        inline size_t size() const {
            return data.size();
        }

        size_t total_length() const;

        size_t linear_offset(const piece_t& piece) const;

        piece_find_result_t find_for_offset(uint32_t offset);

        std::list<piece_t> data {};
    };

    struct piece_table_buffer_t {
        enum types {
            original,
            changes
        };

        types type = types::original;
        std::vector<element_t> elements {};

        void clear() {
            elements.clear();
        }

        inline size_t size() const {
            return elements.size();
        }

        inline bool empty() const {
            return elements.empty();
        }
    };

    struct piece_table_t {
        void clear();

        void delete_at(
                const document_position_t& position,
                size_t length);

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
                const document_position_t& position);

        attr_line_list sequence();

        void load(const piece_table_buffer_t& buffer);

        piece_list_t pieces {};
        attr_t default_attr {};
        piece_table_buffer_t original {};
        piece_table_buffer_t changes {piece_table_buffer_t::types::changes};
    };

};

