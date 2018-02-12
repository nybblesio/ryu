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
#include <stack>
#include <string>
#include <vector>
#include <cstdint>

namespace ryu::core {

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

        void delete_at(uint32_t offset, uint32_t length);

        void insert(uint32_t offset, const element_t& element);

        const attr_line_list& sequence();

        void load(const piece_table_buffer_t& buffer);

        piece_list_t pieces {};
        attr_t default_attr {};
        attr_line_list lines {};
        piece_table_buffer_t original {};
        std::stack<piece_t*> undo_stack {};
        std::stack<piece_t*> redo_stack {};
        piece_table_buffer_t changes {piece_table_buffer_t::types::changes};
    };

};

