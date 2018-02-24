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
#include <iostream>
#include "piece_table.h"

namespace ryu::core {

    void piece_node_t::copy_elements(
            attr_span_list_t& spans,
            uint32_t begin,
            uint32_t end) {
        attr_span_t* current_span = nullptr;
        if (!spans.empty()) {
            current_span = &spans.back();
        }

        std::stringstream stream {};
        for (size_t i = begin; i < end; i++) {
            auto& element = buffer->elements[buffer_start + i];
            if (current_span != nullptr) {
                if (current_span->attr != element.attr
                ||  offset.start > current_span->column_offset_end) {
                    current_span->text += stream.str();
                    stream.str("");
                    spans.push_back(attr_span_t {
                        element.attr,
                        "",
                        offset.start,
                        offset.end,
                        offset.end});
                    current_span = &spans.back();
                }
            } else {
                spans.push_back(attr_span_t {
                    element.attr,
                    "",
                    offset.start,
                    offset.end,
                    offset.end});
                current_span = &spans.back();
            }

            element.safe_value(stream);
        }

        if (current_span != nullptr) {
            current_span->text += stream.str();
            current_span->column_offset_end = offset.end;
        }
    }

    void piece_node_t::copy_elements(attr_span_list_t& spans) {
        copy_elements(spans, 0, length);
    }

    ///////////////////////////////////////////////////////////////////////////

    void piece_table::paste(
            const selection_t& selection,
            const element_list_t& elements) {
        delete_at(selection.line, selection.start, selection.length);
        insert_at(selection.line, selection.start, elements);
    }

    void piece_table::clear() {
        _pieces.clear();
        _changes.clear();
        _original.clear();
        if (_undo_manager != nullptr)
            _undo_manager->clear();
    }

    void piece_table::insert_at(
            uint32_t row,
            uint32_t column,
            const element_list_t& elements) {
        if (elements.empty())
            return;

        auto new_change_offset = _changes.size();
        size_t element_count = elements.size();
        auto find_result = _pieces.find_for_insert(
            row,
            column,
            static_cast<uint32_t>(element_count));

        for (const auto& element : elements)
            _changes.elements.push_back(element);

        auto end_column = static_cast<uint32_t>(column + element_count);

        switch (find_result.type) {
            case piece_find_result_t::append: {
                auto cloned_piece = _pieces.clone_and_swap(
                    find_result.data,
                    _undo_manager);
                cloned_piece->length += element_count;
                cloned_piece->offset.end += element_count;
                update_sibling_offsets(
                    cloned_piece,
                    static_cast<int32_t>(element_count));
                break;
            }
            case piece_find_result_t::none:
            case piece_find_result_t::insert: {
                auto node = std::make_shared<piece_node_t>(
                    piece_node_t::types::regular,
                    row,
                    new_change_offset,
                    element_count,
                    offset_t {column, end_column},
                    &_changes);
                auto cloned_node = _pieces.clone_and_swap(
                    find_result.data,
                    _undo_manager);
                _pieces.insert_before(cloned_node, node);
                update_sibling_offsets(
                    node.get(),
                    static_cast<int32_t>(element_count));
                break;
            }
            case piece_find_result_t::splice: {
                auto node = std::make_shared<piece_node_t>(
                    piece_node_t::types::regular,
                    row,
                    new_change_offset,
                    element_count,
                    offset_t {column, end_column},
                    &_changes);
                auto cloned_node = _pieces.clone_and_swap(
                    find_result.data,
                    _undo_manager);
                _pieces.insert_before(cloned_node, node);
                update_sibling_offsets(
                    node.get(),
                    static_cast<int32_t>(element_count));
                break;
            }
            case piece_find_result_t::split: {
                auto new_piece = std::make_shared<piece_node_t>(
                    piece_node_t::types::regular,
                    row,
                    static_cast<uint32_t>(new_change_offset),
                    element_count,
                    offset_t {column, end_column},
                    &_changes);

                auto left_piece = _pieces.clone_and_swap(
                    find_result.data,
                    _undo_manager);

                int32_t right_piece_length = std::max<int32_t>(
                    static_cast<int32_t>(left_piece->offset.end - column),
                    0);
                auto right_piece = std::make_shared<piece_node_t>(
                    piece_node_t::types::regular,
                    row,
                    left_piece->buffer_start + (column - left_piece->offset.start),
                    right_piece_length,
                    offset_t {end_column, end_column + right_piece_length},
                    left_piece->buffer
                );

                int32_t left_piece_length = std::max<int32_t>(
                    static_cast<int32_t>(left_piece->offset.end - column),
                    0);
                left_piece->length -= left_piece_length;
                left_piece->offset.end -= left_piece_length;

                _pieces.insert_after(left_piece, new_piece);
                _pieces.insert_after(new_piece.get(), right_piece);

                update_sibling_offsets(
                    right_piece.get(),
                    static_cast<int32_t>(element_count));
                break;
            }
            default:
                // XXX: should never happen
                break;
        }
    }

    void piece_table::delete_at(
            uint32_t row,
            uint32_t column,
            uint32_t length) {
        if (length == 0)
            return;

        auto line = _pieces.get_line(row);
        auto line_total_length = line->total_length();
        if (column >= line_total_length)
            return;

        while (true) {
            auto find_result = _pieces.find_for_delete(
                row,
                column,
                length);
            if (find_result.type == piece_find_result_t::none)
                break;

            switch(find_result.type) {
                case piece_find_result_t::split: {
                    auto relative_start_offset = column - find_result.data->offset.start;
                    auto relative_length = relative_start_offset + length;
                    auto new_buffer_start = find_result.data->buffer_start + relative_length;
                    auto new_piece_length = find_result.data->length - relative_length;
                    auto new_piece = std::make_shared<piece_node_t>(
                        piece_node_t::types::regular,
                        row,
                        new_buffer_start,
                        new_piece_length,
                        offset_t {column, column + new_piece_length},
                        &_changes);

                    auto cloned_piece = _pieces.clone_and_swap(
                        find_result.data,
                        _undo_manager);
                    cloned_piece->length = relative_start_offset;

                    _pieces.insert_after(cloned_piece, new_piece);

                    update_sibling_offsets(new_piece.get(), -new_piece_length);
                    break;
                }
                case piece_find_result_t::splice: {
                    if (_undo_manager != nullptr) {
                        _undo_manager->swap_deleted_node(
                            _pieces,
                            find_result.data);
                    }
                    break;
                }
                case piece_find_result_t::shrink: {
                    auto clone_node = _pieces.clone_and_swap(
                        find_result.data,
                        _undo_manager);
                    clone_node->length -= length;
                    break;
                }
                case piece_find_result_t::squeeze: {
                    auto clone_node = _pieces.clone_and_swap(
                        find_result.data,
                        _undo_manager);
                    clone_node->buffer_start += length;
                    clone_node->length -= length;
                    break;
                }
                default:
                    break;
            }

            update_sibling_offsets(
                find_result.data,
                -length);

            column += find_result.data->length - length;
            length -= find_result.data->length;
        }
    }

    position_t piece_table::undo() {
        if (_undo_manager != nullptr)
            return _undo_manager->undo(_pieces);
        return {};
    }

    position_t piece_table::redo() {
        if (_undo_manager != nullptr)
            return _undo_manager->redo(_pieces);
        return {};
    }

    void piece_table::checkpoint() {
        if (_undo_manager != nullptr)
            _undo_manager->clear();
    }

    attr_span_list_t piece_table::sub_sequence(
            uint32_t row,
            uint32_t start,
            uint32_t end) {
        attr_span_list_t spans;

        auto line = _pieces.get_line(row);
        auto current_node = line->head;

        while (current_node != line->tail) {
            if (current_node->offset.start < start && current_node->offset.end < start) goto next_node;
            if (current_node->offset.start > end) break;

            if (end <= current_node->offset.end) {
                if (current_node->offset.within(start)) {
                    auto start_offset = start - current_node->offset.start;
                    auto end_offset = std::min<uint32_t>(current_node->length, end);
                    current_node->copy_elements(
                        spans,
                        start_offset,
                        end_offset);
                } else {
                    current_node->copy_elements(
                        spans,
                        0,
                        end - current_node->offset.start);
                }
            } else if (start <= current_node->offset.start) {
                current_node->copy_elements(spans);
            } else if (start <= current_node->offset.end) {
                auto start_offset = start - current_node->offset.start;
                current_node->copy_elements(spans, start_offset, current_node->length);
            }

        next_node:
            current_node = current_node->next;
        }

        return spans;
    }

    void piece_table::update_sibling_offsets(
        piece_node_t* head,
        int32_t length) {
        if (head == nullptr)
            return;
        head = head->next;
        while (head != nullptr) {
            head->offset.start += length;
            head->offset.end = head->offset.start + head->length;
            head = head->next;
        }
    }

    const selection_t& piece_table::add_selection(
            selection_t::types type,
            uint32_t row,
            uint32_t start,
            uint32_t length,
            const std::string& name) {
        _selections.push_back(selection_t{
            name,
            row,
            start,
            length,
            type});
        return _selections.back();
    }

    attr_span_list_t piece_table::sequence(uint32_t row) {
        auto line = _pieces.get_line(row);
        return sub_sequence(row, 0, line->total_length());
    }

    const selection_list& piece_table::selections() const {
        return _selections;
    }

    piece_table_undo_manager* piece_table::undo_manager() {
        return _undo_manager;
    }

    void piece_table::load(const piece_table_buffer_t& buffer) {
        clear();
        _original = buffer;

        if (!buffer.empty()) {
            auto new_piece = std::make_shared<piece_node_t>(
                piece_node_t::types::regular,
                0,
                0,
                _original.elements.size(),
                offset_t {},
                &_original);
            _pieces.insert_head(new_piece);
        }
    }

    void piece_table::undo_manager(piece_table_undo_manager* value) {
        _undo_manager = value;
    }

    void piece_table::remove_selection(const selection_t& selection) {
        _selections.erase(std::remove(
                _selections.begin(),
                _selections.end(),
                selection),
            _selections.end());
    }

    attr_span_list_t piece_table::cut(const selection_t& selection) {
        auto spans = copy(selection);
        delete_selection(selection);
        return spans;
    }

    attr_span_list_t piece_table::copy(const selection_t& selection) {
        return sub_sequence(selection.line, selection.start, selection.start + selection.length);
    }

    void piece_table::delete_selection(const selection_t& selection) {
        delete_at(selection.line, selection.start, selection.length);
    }

    ///////////////////////////////////////////////////////////////////////////

    piece_list::piece_list() {
        clear();
    }

    void piece_list::clear() {
        _lines.clear();
        _owned_pieces.clear();
        auto sentinels = make_sentinels();
        _head = sentinels.first;
        _tail = sentinels.second;
    }

    void piece_list::insert_after(
            piece_node_t* node,
            const piece_node_shared_ptr& piece) {
        _owned_pieces.insert(piece);

        const auto piece_raw_ptr = piece.get();
        auto node_next = node->next;
        piece_raw_ptr->next = node_next;
        piece_raw_ptr->prev = node;
        node->next = piece_raw_ptr;
        node_next->prev = piece_raw_ptr;
    }

    void piece_list::insert_before(
            piece_node_t* node,
            const piece_node_shared_ptr& piece) {
        _owned_pieces.insert(piece);

        const auto piece_raw_ptr = piece.get();
        auto node_prev = node->prev;
        piece_raw_ptr->next = node;
        piece_raw_ptr->prev = node_prev;
        node->prev = piece_raw_ptr;
        node_prev->next = piece_raw_ptr;
    }

    size_t piece_list::size() const {
        size_t count = 0;
        auto current_node = _head;
        while (current_node != nullptr) {
            ++count;
            current_node = current_node->next;
        }
        return count;
    }

    piece_node_t* piece_list::clone(piece_node_t* node) {
        auto clone_node = std::make_shared<piece_node_t>(
            node->type,
            node->line,
            node->buffer_start,
            node->length,
            node->offset,
            node->buffer);
        clone_node->next = node->next;
        clone_node->prev = node->prev;
        _owned_pieces.insert(clone_node);
        return clone_node.get();
    }

    piece_node_t* piece_list::clone_and_swap(
            piece_node_t* node,
            piece_table_undo_manager* undo_manager) {
        if (undo_manager != nullptr)
            undo_manager->push_undo(node);

        auto clone_node = std::make_shared<piece_node_t>(
            node->type,
            node->line,
            node->buffer_start,
            node->length,
            node->offset,
            node->buffer);
        _owned_pieces.insert(clone_node);

        auto clone_raw_ptr = clone_node.get();

        auto line = get_line(node->line);
        if (line->head == node)
            line->head = clone_raw_ptr;
        else if (line->tail == node)
            line->tail = clone_raw_ptr;

        auto node_prev = node->prev;
        auto node_next = node->next;

        if (node_prev != nullptr)
            node_prev->next = clone_raw_ptr;

        if (node_next != nullptr)
            node_next->prev = clone_raw_ptr;

        clone_raw_ptr->next = node_next;
        clone_raw_ptr->prev = node_prev;

        return clone_raw_ptr;
    }

    piece_find_result_t piece_list::find_for_delete(
            uint32_t row,
            uint32_t column,
            uint32_t length) {
        auto line = get_line(row);
        auto current_node = line->head->next;

        while (current_node != line->tail) {
            if (current_node->offset.within(column)) {
                auto result = piece_find_result_t::create(
                    line,
                    current_node);
                if (column > current_node->offset.start)
                    result.type = piece_find_result_t::squeeze;
                else if (column < current_node->offset.end)
                    result.type = piece_find_result_t::shrink;
                else
                    result.type = piece_find_result_t::splice;
                return result;
            } else {
                auto prev_node = current_node->prev;
                if (prev_node != nullptr
                &&  column > prev_node->offset.end) {
                    return piece_find_result_t::create(
                        line,
                        current_node,
                        piece_find_result_t::shift);
                }
            }
            current_node = current_node->next;
        }

        return {};
    }

    piece_find_result_t piece_list::find_for_insert(
            uint32_t row,
            uint32_t column,
            uint32_t length) {
        auto line = get_line(row);
        auto current_node = line->head->next;

        while (current_node != line->tail) {
            auto prev_node = current_node->prev;
            if (column < current_node->offset.start
            &&  prev_node == nullptr) {
                return piece_find_result_t::create(
                    line,
                    current_node,
                    piece_find_result_t::insert);
            } else if (current_node->offset.within(column)) {
                auto result = piece_find_result_t::create(
                    line,
                    current_node);

                auto next_buffer_offset = current_node->buffer->size();

                if (current_node->offset.end == column
                &&  next_buffer_offset == current_node->buffer_end()
                &&  current_node->buffer->type == piece_table_buffer_t::types::changes) {
                    result.type = piece_find_result_t::types::append;
                } else if (current_node->offset.start == column) {
                    result.type = piece_find_result_t::types::insert;
                } else {
                    result.type = piece_find_result_t::types::split;
                }

                return result;
            } else {
                auto end_column = column + length;
                if (prev_node != nullptr) {
                    if (column > prev_node->offset.end
                    &&  end_column < current_node->offset.start) {
                        return piece_find_result_t::create(
                            line,
                            current_node,
                            piece_find_result_t::insert);
                    }
                }
            }
            current_node = current_node->next;
        }

        if (!line->empty()) {
            return piece_find_result_t::create(
                line,
                line->tail,
                piece_find_result_t::insert);
        }

        return {};
    }

    piece_line_t* piece_list::get_line(uint32_t line) {
        if (line >= _lines.size())
            adjust_lines_to_fit(line);
        return &_lines[line];
    }

    void piece_list::adjust_lines_to_fit(uint32_t line) {
        auto missing_line_count = std::max<size_t>(0, (line - _lines.size()) + 1);
        for (auto i = 0; i < missing_line_count; i++) {
            auto sentinels = make_sentinels();

            piece_line_t new_line {};
            new_line.head = sentinels.first;
            new_line.head->type = piece_node_t::types::line_head_sentinel;
            new_line.tail = sentinels.second;
            new_line.tail->type = piece_node_t::types::line_tail_sentinel;

            if (line == 0) {
                new_line.head->prev = _head;
                _head->next = new_line.head;
            }

            if (line + i == _lines.size()) {
                new_line.tail->next = _tail;
                _tail->prev = new_line.tail;
            }

            _lines.push_back(new_line);
        }
    }

    void piece_list::insert_head(const piece_node_shared_ptr& piece) {
        insert_after(_head, piece);
    }

    void piece_list::insert_tail(const piece_node_shared_ptr& piece) {
        insert_before(_tail, piece);
    }

    std::pair<piece_node_t*, piece_node_t*> piece_list::make_sentinels() {
        auto head_sentinel = std::make_shared<piece_node_t>(
            piece_node_t::types::head_sentinel,
            0,
            0,
            0,
            offset_t {},
            nullptr);
        auto tail_sentinel = std::make_shared<piece_node_t>(
            piece_node_t::types::tail_sentinel,
            0,
            0,
            0,
            offset_t {},
            nullptr);
        _owned_pieces.insert(head_sentinel);
        _owned_pieces.insert(tail_sentinel);

        auto head = head_sentinel.get();
        auto tail = tail_sentinel.get();
        head->next = tail;
        tail->prev = head;

        return std::make_pair(head, tail);
    }

    ///////////////////////////////////////////////////////////////////////////

    void piece_table_undo_manager::clear() {
        clear_stack(_undo);
        clear_stack(_redo);
    }

    void piece_table_undo_manager::swap_deleted_node(
            piece_list& pieces,
            piece_node_t* node) {
        _undo.push(node);
        auto prev_node = node->prev;
        node->prev->next = node->next;
        node->next->prev = prev_node;
    }

    piece_node_t* piece_table_undo_manager::swap_node(
            piece_list& pieces,
            piece_node_t* node) {
        auto prev_node = node->prev;
        auto next_node = node->next;
        prev_node->next = node;
        next_node->prev = node;
        return node;
    }

    void piece_table_undo_manager::push_undo(piece_node_t* node) {
        _undo.push(node);
    }

    void piece_table_undo_manager::push_redo(piece_node_t* node) {
        _redo.push(node);
    }

    position_t piece_table_undo_manager::redo(piece_list& pieces) {
        if (_redo.empty())
            return {};

        auto node = _redo.top();
        if (_redo.size() == 1) {
            _redo.pop();
            pieces.clear();
        } else {
            _undo.push(node);
            swap_node(pieces, node);
            _redo.pop();
        }

        auto line = pieces.get_line(node->line);
        return {
            static_cast<int32_t>(node->line),
            static_cast<int32_t>(line->total_length())
        };
    }

    position_t piece_table_undo_manager::undo(piece_list& pieces) {
        if (_undo.empty())
            return {};

        auto node = _undo.top();
        if (_undo.size() == 1) {
            _undo.pop();
            pieces.clear();
        } else {
            auto undone_node = swap_node(pieces, node);
            _redo.push(undone_node);
            _undo.pop();
        }

        auto line = pieces.get_line(node->line);
        return {
            static_cast<int32_t>(node->line),
            static_cast<int32_t>(line->total_length())
        };
    }

    void piece_table_undo_manager::clear_stack(piece_node_stack& stack) {
        while (!stack.empty())
            stack.pop();
    }

};