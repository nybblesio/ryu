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
#include "core_types.h"

namespace ryu::core {

    struct offset_t {
        uint32_t start = 0;
        uint32_t end = 0;

        bool within(uint32_t value) const {
            return value >= start && value <= end;
        }
    };

    struct piece_node_t;

    using piece_node_shared_ptr = std::shared_ptr<piece_node_t>;

    struct piece_table_buffer_t;

    struct piece_node_t {
        piece_node_t(uint32_t start, uint32_t length, piece_table_buffer_t* buffer) {
            this->start = start;
            this->length = length;
            this->buffer = buffer;
        }

        uint32_t start {};
        uint32_t length {};
        offset_t offset {};
        piece_node_t* prev = nullptr;
        piece_node_t* next = nullptr;
        piece_table_buffer_t* buffer = nullptr;

        inline size_t end() const {
            return start + length;
        }

        void copy_elements(
                attr_line_list& lines,
                uint32_t begin,
                uint32_t end);

        void copy_elements(attr_line_list& lines);
    };

    struct piece_find_result_t {
        enum types {
            none,
            first,
            final,
            medial
        };
        types type = types::none;
        piece_node_t* data = nullptr;
        offset_t offset {};
    };

    using piece_node_stack = std::stack<piece_node_t*>;
    using piece_node_shared_ptr_set = std::set<piece_node_shared_ptr>;

    class piece_list_t {
    public:
        void undo();

        void redo();

        void clear();

        void checkpoint();

        size_t size() const;

        void update_offsets();

        inline bool empty() const {
            return _head == nullptr;
        }

        size_t undo_depth() const {
            return _undo_stack.size();
        }

        size_t redo_depth() const {
            return _redo_stack.size();
        }

        uint32_t total_length() const {
            if (_tail != nullptr)
                return _tail->offset.end;
            return 0;
        }

        inline piece_node_t* head() const {
            return _head;
        }

        inline piece_node_t* tail() const {
            return _tail;
        }

        void clear_stack(piece_node_stack& stack);

        void swap_deleted_node(piece_node_t* node);

        piece_node_t* clone_and_swap(piece_node_t* node);

        void add_tail(const piece_node_shared_ptr& piece);

        void insert_head(const piece_node_shared_ptr& piece);

        piece_find_result_t find_for_offset(uint32_t offset);

        void insert_after(piece_node_t* node, const piece_node_shared_ptr& piece);

        void insert_before(piece_node_t* node, const piece_node_shared_ptr& piece);

    protected:
        void swap_node(
                piece_node_t* node,
                piece_node_stack& target_stack);

    private:
        piece_node_t* _head = nullptr;
        piece_node_t* _tail = nullptr;
        piece_node_stack _undo_stack {};
        piece_node_stack _redo_stack {};
        piece_node_shared_ptr_set _owned_pieces {};
    };

    struct piece_table_buffer_t {
        enum types {
            original,
            changes
        };

        types type = types::original;
        element_list elements {};

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

    struct selection_t {
        enum types {
            clipboard,
            mark,
            custom
        };

        std::string name {};
        uint32_t start = 0;
        uint32_t length = 0;
        types type = types::clipboard;

        bool operator== (const selection_t& other) {
            return type == other.type
                    && name == other.name
                    && start == other.start
                    && length == other.length;
        }

        bool operator!= (const selection_t& other) {
            return !(*this == other);
        }
    };

    using selection_list = std::vector<selection_t>;

    class piece_table_t {
    public:
        void undo();

        void redo();

        void clear();

        void rebuild();

        void checkpoint();

        uint32_t total_length() const {
            return _pieces.total_length();
        }

        const attr_line_list& sequence();

        const selection_t& add_selection(
                selection_t::types type,
                uint32_t start,
                uint32_t length,
                const std::string& name = "");

        const piece_list_t& pieces() const {
            return _pieces;
        }

        const selection_list& selections() const;

        const piece_table_buffer_t& changes() const {
            return _changes;
        }

        void load(const piece_table_buffer_t& buffer);

        const piece_table_buffer_t& original() const {
            return _original;
        }

        attr_line_list cut(const selection_t& selection);

        void delete_at(uint32_t offset, uint32_t length);

        attr_line_list copy(const selection_t& selection);

        void delete_selection(const selection_t& selection);

        void remove_selection(const selection_t& selection);

        attr_line_list sub_sequence(uint32_t start, uint32_t end);

        void insert_at(uint32_t offset, const element_list_t& elements);

        void paste(const selection_t& selection, const element_list_t& elements);

    private:
        piece_list_t _pieces {};
        attr_line_list _lines {};
        selection_list _selections {};
        piece_table_buffer_t _original {};
        piece_table_buffer_t _changes {piece_table_buffer_t::types::changes};
    };

};

