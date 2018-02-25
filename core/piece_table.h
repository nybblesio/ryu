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

    struct position_t {
        int32_t row = -1;
        int32_t column = -1;

        inline bool empty() const {
            return row == -1 && column == -1;
        }
    };

    struct offset_t {
        uint32_t start = 0;
        uint32_t end = 0;

        bool within(uint32_t value) const {
            return value >= start && value <= end;
        }

        bool same(uint32_t start, uint32_t end) const {
            return this->start == start && this->end == end;
        }
    };

    struct piece_node_t;

    using piece_node_shared_ptr = std::shared_ptr<piece_node_t>;

    struct piece_table_buffer_t;

    struct piece_node_t {
        enum types {
            head_sentinel,
            tail_sentinel,
            line_head_sentinel,
            line_tail_sentinel,
            regular
        };

        piece_node_t(
                piece_node_t::types type,
                uint32_t line,
                uint32_t buffer_start,
                uint32_t length,
                const offset_t& offset,
                piece_table_buffer_t* buffer) {
            this->type = type;
            this->line = line;
            this->offset = offset;
            this->length = length;
            this->buffer = buffer;
            this->buffer_start = buffer_start;
        }

        uint32_t line {};
        uint32_t buffer_start {};
        uint32_t length {};
        offset_t offset {};
        types type = types::regular;
        piece_node_t* prev = nullptr;
        piece_node_t* next = nullptr;
        piece_table_buffer_t* buffer = nullptr;

        bool is_sentinel() const {
            return  type == types::head_sentinel
                || type == types::tail_sentinel
                || type == types::line_head_sentinel
                || type == types::line_tail_sentinel;
        }

        inline size_t buffer_end() const {
            return buffer_start + length;
        }

        void copy_elements(
                attr_span_list_t& spans,
                uint32_t begin,
                uint32_t end);

        void copy_elements(attr_span_list_t& spans);
    };

    struct piece_line_t {
        piece_node_t* head;
        piece_node_t* tail;

        inline bool empty() const {
            return head->next == tail && tail->prev == head;
        }

        uint32_t total_length() const {
            return tail->offset.end;
        }
    };

    struct piece_find_result_t {
        enum types {
            none,
            split,
            shift,
            splice,
            append,
            insert,
            shrink,
            squeeze
        };

        static piece_find_result_t create(
                piece_line_t* line,
                piece_node_t* node,
                piece_find_result_t::types type = types::none) {
            piece_find_result_t result {};
            result.type = type;
            result.data = node;
            result.offset = node->offset;
            result.is_first = node->prev == line->head;
            result.is_final = node->next == line->tail;
            return result;
        }

        bool is_first = false;
        bool is_final = false;
        types type = types::none;
        piece_node_t* data = nullptr;
        offset_t offset {};
    };

    class piece_table_undo_manager;

    using piece_node_shared_ptr_set = std::set<piece_node_shared_ptr>;

    class piece_list {
    public:
        piece_list();

        void clear();

        void insert_after(
            piece_node_t* node,
            const piece_node_shared_ptr& piece);

        void insert_before(
            piece_node_t* node,
            const piece_node_shared_ptr& piece);

        size_t size() const;

        inline bool empty() const {
            return _head->next == _tail && _tail->prev == _head;
        }

        uint32_t total_length() const {
            return _tail->offset.end;
        }

        piece_node_t* clone_and_swap(
            piece_node_t* node,
            piece_table_undo_manager* undo_manager);

        piece_find_result_t find_for_delete(
            uint32_t row,
            uint32_t column,
            uint32_t length);

        piece_find_result_t find_for_insert(
            uint32_t row,
            uint32_t column,
            uint32_t length);

        piece_line_t* get_line(uint32_t line);

        piece_node_t* clone(piece_node_t* node);

        void insert_head(const piece_node_shared_ptr& piece);

        void insert_tail(const piece_node_shared_ptr& piece);

    private:
        void adjust_lines_to_fit(uint32_t line);

        std::pair<piece_node_t*, piece_node_t*> make_sentinels();

    private:
        friend class piece_table_undo_manager;

        piece_node_t* _head;
        piece_node_t* _tail;
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
        void paste(
            const selection_t& selection,
            const element_list_t& elements);

        void clear();

        position_t undo();

        position_t redo();

        void checkpoint();

        void insert_at(
            uint32_t row,
            uint32_t column,
            const element_list_t& elements);

        void delete_at(
            uint32_t row,
            uint32_t column,
            int32_t length);

        attr_span_list_t sub_sequence(
            uint32_t row,
            uint32_t start,
            uint32_t end);

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

        void undo_manager(piece_table_undo_manager* value);

        attr_span_list_t cut(const selection_t& selection);

        attr_span_list_t copy(const selection_t& selection);

        void delete_selection(const selection_t& selection);

        void remove_selection(const selection_t& selection);

    private:
        void update_sibling_offsets(
            piece_node_t* head,
            int32_t length);

    private:
        friend class piece_table_undo_manager;

        piece_list _pieces;
        selection_list _selections {};
        piece_table_buffer_t _changes {
            piece_table_buffer_t::types::changes
        };
        piece_table_buffer_t _original {};
        piece_table_undo_manager* _undo_manager = nullptr;
    };

    using piece_node_stack = std::stack<piece_node_t*>;

    class piece_table_undo_manager {
    public:
        void clear();

        void swap_deleted_node(
                piece_list& pieces,
                piece_node_t* node);

        size_t undo_depth() const {
            return _undo.size();
        }

        size_t redo_depth() const {
            return _redo.size();
        }

        void push_undo(piece_node_t* node);

        void push_redo(piece_node_t* node);

        position_t redo(piece_list& pieces);

        position_t undo(piece_list& pieces);

    protected:
        piece_node_t* swap_node(
                piece_list& pieces,
                piece_node_t* node);

        void clear_stack(piece_node_stack& stack);

    private:
        piece_node_stack _undo {};
        piece_node_stack _redo {};
    };

};