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
    };

    //
    //       +---------+
    //       |  piece  |
    //  <--  |         | -->   ...  ...  ...
    //  prev +---------+ next
    //
    // (chain 1) (current) nullptr <- piece1 <-> piece2 <-> piece3 -> nullptr
    //
    // 1. insert at beginning of piece2
    //
    // (chain 2) (current) nullptr <- piece1.1 <-> piece2.1 <-> piece3.1 -> nullptr
    // push (chain 1) (undo stack)
    //
    // chain or span is
    //
    // 2. insert into middle of piece2.1
    //
    // (chain 3) (current) nullptr <- piece1.1 <-> piece4(left of 2.1) <-> piece5 <-> piece6(right of 2.1) <-> piece3 -> nullptr
    //
    //
    // 0 <-> a <-> b <-> c <-> d <-> e <-> f <-> g -> 0
    //                         ^
    //                         |---> stash this pointer on undo stack
    //
    // d->prev still == c
    // c->next DOES NOT == d
    //
    // d->next still == e
    // e->prev DOES NOT == d
    //
    //
    // next
    //
    // 0 <-> a <-> b <-> c <-> d_left <-> new <-> d_right <-> e <-> f <-> g -> 0
    //
    // now
    //
    // c->next == d_left
    // e->prev == d_right
    //

    using piece_node_stack = std::stack<piece_node_t*>;
    using piece_node_shared_ptr_set = std::set<piece_node_shared_ptr>;

    struct piece_list_t {
        piece_node_t* head = nullptr;
        piece_node_t* tail = nullptr;
        piece_node_stack undo_stack {};
        piece_node_stack redo_stack {};
        piece_node_shared_ptr_set owned_pieces {};

        inline void clear() {
            head = nullptr;
        }

        inline bool empty() const {
            return head == nullptr;
        }

        void undo();

        void redo();

        void checkpoint();

        size_t size() const;

        size_t total_length() const;

        void swap_node(
                piece_node_t* node,
                piece_node_stack& target_stack);

        void clear_stack(piece_node_stack& stack);

        piece_node_t* clone_and_swap(piece_node_t* node);

        void add_tail(const piece_node_shared_ptr& piece);

        void insert_head(const piece_node_shared_ptr& piece);

        piece_find_result_t find_for_offset(uint32_t offset);

        size_t linear_offset(const piece_node_t* start_node) const;

        void insert_after(piece_node_t* node, const piece_node_shared_ptr& piece);

        void insert_before(piece_node_t* node, const piece_node_shared_ptr& piece);
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

    struct piece_table_t {
        void clear();

        void undo();

        void redo();

        void checkpoint();

        const attr_line_list& sequence();

        const selection_t& add_selection(
                selection_t::types type,
                uint32_t start,
                uint32_t length,
                const std::string& name = "");

        void swap_deleted_node(piece_node_t* node);

        void load(const piece_table_buffer_t& buffer);

        attr_line_list cut(const selection_t& selection);

        void delete_at(uint32_t offset, uint32_t length);

        attr_line_list copy(const selection_t& selection);

        void delete_selection(const selection_t& selection);

        void remove_selection(const selection_t& selection);

        attr_line_list sub_sequence(uint32_t start, uint32_t end);

        void insert_at(uint32_t offset, const element_t& element);

        void paste(const selection_t& selection, const element_list& elements);

        piece_list_t pieces {};
        attr_line_list lines {};
        selection_list selections {};
        piece_table_buffer_t original {};
        piece_table_buffer_t changes {piece_table_buffer_t::types::changes};
    };

};

