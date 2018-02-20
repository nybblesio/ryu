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
        piece_node_t(
                uint32_t line,
                uint32_t buffer_start,
                uint32_t length,
                const offset_t& offset,
                piece_table_buffer_t* buffer) {
            this->buffer_start = buffer_start;
            this->offset = offset;
            this->length = length;
            this->buffer = buffer;
            this->line = line;
        }

        uint32_t line {};
        uint32_t buffer_start {};
        uint32_t length {};
        offset_t offset {};
        piece_node_t* prev = nullptr;
        piece_node_t* next = nullptr;
        piece_table_buffer_t* buffer = nullptr;

        inline size_t buffer_end() const {
            return buffer_start + length;
        }

        void copy_elements(
                attr_span_list_t& spans,
                uint32_t begin,
                uint32_t end);

        void copy_elements(attr_span_list_t& spans);
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

    class piece_table_undo_manager;

    using piece_node_shared_ptr_set = std::set<piece_node_shared_ptr>;

    struct piece_line_t {
        piece_node_t* head = nullptr;
        piece_node_t* tail = nullptr;

        uint32_t total_length() const {
            if (tail != nullptr)
                return tail->offset.end;
            return 0;
        }
    };

    class piece_list {
    public:
        void clear();

        size_t size() const;

        void update_offsets();

        inline bool empty() const {
            return _head == nullptr;
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

        piece_line_t* get_line(uint32_t line);

        void add_tail(
                const piece_node_shared_ptr& piece,
                piece_table_undo_manager* undo_manager);

        void insert_head(
                const piece_node_shared_ptr& piece,
                piece_table_undo_manager* undo_manager);

        piece_node_t* clone_and_swap(
                piece_node_t* node,
                piece_table_undo_manager* undo_manager);

        piece_find_result_t find_for_offset(uint32_t row, uint32_t column);

        void insert_after(piece_node_t* node, const piece_node_shared_ptr& piece);

        void insert_before(piece_node_t* node, const piece_node_shared_ptr& piece);

    private:
        void adjust_lines_to_fit(uint32_t line);

    private:
        friend class piece_table_undo_manager;

        piece_node_t* _head = nullptr;
        piece_node_t* _tail = nullptr;
        std::vector<piece_line_t> _lines {};
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
        uint32_t line = 0;
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

    class piece_table {
    public:
        void undo();

        void redo();

        void clear();

        void checkpoint();

        uint32_t total_length() const {
            return _pieces.total_length();
        }

        const selection_t& add_selection(
                selection_t::types type,
                uint32_t row,
                uint32_t start,
                uint32_t length,
                const std::string& name = "");

        const piece_list& pieces() const {
            return _pieces;
        }

        attr_span_list_t sequence(uint32_t row);

        piece_table_undo_manager* undo_manager();

        const selection_list& selections() const;

        const piece_table_buffer_t& changes() const {
            return _changes;
        }

        void load(const piece_table_buffer_t& buffer);

        const piece_table_buffer_t& original() const {
            return _original;
        }

        attr_span_list_t cut(const selection_t& selection);

        attr_span_list_t copy(const selection_t& selection);

        void undo_manager(piece_table_undo_manager* value);

        void delete_selection(const selection_t& selection);

        void remove_selection(const selection_t& selection);

        void delete_at(uint32_t row, uint32_t column, uint32_t length);

        void paste(const selection_t& selection, const element_list_t& elements);

        attr_span_list_t sub_sequence(uint32_t row, uint32_t start, uint32_t end);

        void insert_at(uint32_t row, uint32_t column, const element_list_t& elements);

    private:
        friend class piece_table_undo_manager;

        piece_list _pieces;
        selection_list _selections {};
        piece_table_buffer_t _original {};
        piece_table_undo_manager* _undo_manager = nullptr;
        piece_table_buffer_t _changes {piece_table_buffer_t::types::changes};
    };

    using piece_node_stack = std::stack<piece_node_t*>;

    class piece_table_undo_manager {
    public:
        void clear();

        size_t undo_depth() const {
            return _undo.size();
        }

        size_t redo_depth() const {
            return _redo.size();
        }

        void undo(piece_list& pieces);

        void redo(piece_list& pieces);

        void push_undo(piece_node_t* node);

        void push_redo(piece_node_t* node);

        void swap_deleted_node(piece_list& pieces, piece_node_t* node);

    protected:
        void swap_node(
                piece_list& pieces,
                piece_node_t* node,
                piece_node_stack& target_stack);

        void clear_stack(piece_node_stack& stack);

    private:
        piece_node_stack _undo {};
        piece_node_stack _redo {};
    };

};

