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
                if (current_span->attr != element.attr) {
                    current_span->text += stream.str();
                    stream.str("");
                    spans.push_back(attr_span_t {element.attr});
                    current_span = &spans.back();
                }
            } else {
                spans.push_back(attr_span_t {element.attr});
                current_span = &spans.back();
            }
            element.safe_value(stream);
        }

        if (current_span != nullptr)
            current_span->text += stream.str();
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

        auto line = _pieces.get_line(row);
        auto new_change_offset = _changes.size();
        size_t element_count = elements.size();
        auto find_result = _pieces.find_for_offset(row, column);

        for (const auto& element : elements)
            _changes.elements.push_back(element);

        auto end_column = static_cast<uint32_t>(column + element_count);

        switch (find_result.type) {
            case piece_find_result_t::none: {
                auto node = std::make_shared<piece_node_t>(
                    row,
                    new_change_offset,
                    element_count,
                    offset_t {column, end_column},
                    &_changes);
                _pieces.add_tail(node, _undo_manager);
                break;
            }
            case piece_find_result_t::append: {
                auto cloned_piece = _pieces.clone_and_swap(
                    find_result.data,
                    _undo_manager);
                cloned_piece->length += element_count;
                cloned_piece->offset.end += element_count;
                update_descendant_offsets(
                    line,
                    cloned_piece,
                    static_cast<int32_t>(element_count));
                break;
            }
            case piece_find_result_t::insert: {
                auto node = std::make_shared<piece_node_t>(
                    row,
                    new_change_offset,
                    element_count,
                    offset_t {column, end_column},
                    &_changes);
                _pieces.insert_before(find_result.data, node, _undo_manager);
                update_descendant_offsets(
                    line,
                    node.get(),
                    static_cast<int32_t>(element_count));
                break;
            }
            case piece_find_result_t::splice: {
                auto node = std::make_shared<piece_node_t>(
                    row,
                    new_change_offset,
                    element_count,
                    offset_t {column, end_column},
                    &_changes);
                _pieces.insert_before(
                    find_result.data,
                    node,
                    _undo_manager);
                update_descendant_offsets(
                    line,
                    node.get(),
                    static_cast<int32_t>(element_count));
                break;
            }
            case piece_find_result_t::split: {
                auto new_piece = std::make_shared<piece_node_t>(
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

                _pieces.insert_after(left_piece, new_piece, _undo_manager);
                _pieces.insert_after(new_piece.get(), right_piece, _undo_manager);

                update_descendant_offsets(
                    line,
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

        uint32_t start = column;
        uint32_t end = column + length;

        auto current_node = line->head;

        while (current_node != nullptr) {
            if (current_node->offset.start < start
            &&  current_node->offset.end < start) {
                goto next_node;
            }

            if (current_node->offset.start > end)
                break;

            if (start <= current_node->offset.start
            &&  end >= current_node->offset.end) {
                auto prev_node = current_node->prev;
                if (_undo_manager != nullptr)
                    _undo_manager->swap_deleted_node(_pieces, current_node);
                update_descendant_offsets(line, prev_node, -current_node->length);
                end -= current_node->length;
            } else if (current_node->offset.within(start)) {
                if (start > current_node->offset.start
                &&  end < current_node->offset.end) {
                    auto relative_start_offset = start - current_node->offset.start;
                    auto relative_length = relative_start_offset + length;
                    auto new_buffer_start = current_node->buffer_start + relative_length;
                    auto new_piece_length = current_node->length - relative_length;

                    piece_node_t* start_piece = nullptr;

                    if (new_piece_length > 0
                    &&  new_buffer_start < current_node->buffer_end()) {
                        auto new_piece = std::make_shared<piece_node_t>(
                            row,
                            new_buffer_start,
                            new_piece_length,
                            offset_t {start, start + new_piece_length},
                            &_changes);

                        auto cloned_piece = _pieces.clone_and_swap(current_node, _undo_manager);
                        cloned_piece->length = relative_start_offset;

                        _pieces.insert_after(cloned_piece, new_piece, _undo_manager);

                        start_piece = new_piece.get();
                        end -= new_piece_length;
                        update_descendant_offsets(line, start_piece, -new_piece_length);
                    } else {
                        start_piece = _pieces.clone_and_swap(current_node, _undo_manager);
                        start_piece->length = relative_start_offset;
                        end -= length;
                        update_descendant_offsets(line, start_piece, -length);
                    }
                } else if (end < current_node->offset.end) {
                    auto remove_length = end - current_node->offset.start;
                    auto cloned_node = _pieces.clone_and_swap(current_node, _undo_manager);
                    cloned_node->buffer_start += remove_length;
                    cloned_node->length -= remove_length;
                    cloned_node->offset.end -= remove_length;
                    update_descendant_offsets(line, cloned_node, -remove_length);
                    end -= remove_length;
                } else {
                    auto remove_length = current_node->offset.end - start;
                    auto cloned_node = _pieces.clone_and_swap(current_node, _undo_manager);
                    cloned_node->length -= remove_length;
                    cloned_node->offset.end -= remove_length;
                    update_descendant_offsets(line, cloned_node, -remove_length);
                    end -= remove_length;
                }
            }

        next_node:
            if (current_node == line->tail)
                break;

            current_node = current_node->next;
        }

        auto start_piece = current_node;
        if (start_piece != nullptr)
            start_piece = start_piece->prev;
        if (start_piece == nullptr)
            start_piece = line->head;
        if (start_piece != nullptr && !start_piece->offset.within(start)) {
            update_descendant_offsets(line, start_piece, -length);
        }
    }

    uint32_t piece_table::undo() {
        if (_undo_manager != nullptr)
            return _undo_manager->undo(_pieces);
        return 0;
    }

    uint32_t piece_table::redo() {
        if (_undo_manager != nullptr)
            return _undo_manager->redo(_pieces);
        return 0;
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

        while (current_node != nullptr) {
            auto prev_node = current_node->prev;

            if (current_node->offset.start < start && current_node->offset.end < start) goto next_node;
            if (current_node->offset.start > end) break;

            if (prev_node != nullptr) {
                auto space = static_cast<int32_t>(current_node->offset.start - prev_node->offset.end);
                if (space > 0) {
                    spans.push_back({attr_t{}, std::string(space, ' ')});
                }
            }

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
            if (current_node == line->tail)
                break;

            current_node = current_node->next;
        }

        return spans;
    }

    const selection_t& piece_table::add_selection(
            selection_t::types type,
            uint32_t row,
            uint32_t start,
            uint32_t length,
            const std::string& name) {
        _selections.push_back(selection_t{name, row, start, length, type});
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
                    0,
                    0,
                    _original.elements.size(),
                    offset_t {},
                    &_original);
            _pieces.add_tail(new_piece, _undo_manager);
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

    void piece_table::update_descendant_offsets(
            piece_line_t* line,
            piece_node_t* head,
            int32_t length) {
        if (line == nullptr || head == nullptr)
            return;
        head = head->next;
        while (head != nullptr) {
            head->offset.start += length;
            head->offset.end = head->offset.start + head->length;

            if (head == line->tail)
                break;

            head = head->next;
        }
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

    void piece_list::clear() {
        _head = nullptr;
        _tail = nullptr;
        _lines.clear();
        _owned_pieces.clear();
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

    void piece_list::add_tail(
            const piece_node_shared_ptr& piece,
            piece_table_undo_manager* undo_manager) {
        _owned_pieces.insert(piece);

        auto raw_ptr = piece.get();
        auto line = get_line(raw_ptr->line);

        if (line->head == nullptr) {
            line->head = raw_ptr;
            line->tail = raw_ptr;
        } else {
            if (undo_manager != nullptr)
                undo_manager->push_undo(line->tail);
            raw_ptr->prev = line->tail;
            line->tail->next = raw_ptr;
            line->tail = raw_ptr;
        }

        if (_head == nullptr) {
            _head = raw_ptr;
            _tail = raw_ptr;
            if (undo_manager != nullptr)
                undo_manager->push_undo(nullptr);
        } else {
            if (raw_ptr->line == _lines.size() - 1)
                _tail = raw_ptr;
        }
    }

    void piece_list::insert_head(
            const piece_node_shared_ptr& piece,
            piece_table_undo_manager* undo_manager) {
        _owned_pieces.insert(piece);

        auto raw_ptr = piece.get();
        auto line = get_line(raw_ptr->line);

        if (line->head == nullptr) {
            line->head = raw_ptr;
            line->tail = raw_ptr;
        } else {
            raw_ptr->next = line->head;
            line->head->prev = raw_ptr;
            line->head = raw_ptr;
        }

        if (_head == nullptr) {
            _head = raw_ptr;
            _tail = raw_ptr;
            if (undo_manager != nullptr)
                undo_manager->push_undo(line->tail);
        } else {
            if (raw_ptr->line == 0)
                _head = raw_ptr;
        }
    }

    piece_node_t* piece_list::clone_and_swap(
            piece_node_t* node,
            piece_table_undo_manager* undo_manager) {
        if (undo_manager != nullptr)
            undo_manager->push_undo(node);

        auto clone_node = std::make_shared<piece_node_t>(
                node->line,
                node->buffer_start,
                node->length,
                node->offset,
                node->buffer);
        _owned_pieces.insert(clone_node);

        auto clone_raw_ptr = clone_node.get();
        auto line = get_line(clone_raw_ptr->line);

        if (node->prev != nullptr) {
            node->prev->next = clone_raw_ptr;
            clone_raw_ptr->prev = node->prev;
        } else {
            line->head = clone_raw_ptr;
            if (clone_raw_ptr->line == 0)
                _head = clone_raw_ptr;
        }

        if (node->next != nullptr) {
            node->next->prev = clone_raw_ptr;
            clone_raw_ptr->next = node->next;
        } else {
            line->tail = clone_raw_ptr;
            if (clone_raw_ptr->line == _lines.size() - 1)
                _tail = clone_raw_ptr;
        }

        return clone_raw_ptr;
    }

    piece_find_result_t piece_list::find_for_offset(
            uint32_t row,
            uint32_t column) {
        piece_find_result_t result {};
        if (empty()) {
            return result;
        }

        auto line = get_line(row);

        auto current_node = line->head;

        while (current_node != nullptr) {
            if (current_node->offset.within(column)) {
                result.data = current_node;
                result.offset = current_node->offset;
                result.is_first = current_node->prev == nullptr;
                result.is_final = current_node->next == nullptr;

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

                break;
            } else {
                auto next_node = current_node->next;
                auto prev_node = current_node->prev;

                if (column < current_node->offset.start && prev_node != nullptr) {
                    if (column > prev_node->offset.end) {
                        result.data = current_node;
                        result.offset = current_node->offset;
                        result.is_first = current_node->prev == nullptr;
                        result.is_final = current_node->next == nullptr;
                        result.type = piece_find_result_t::types::splice;
                        break;
                    }
                }

                if (column > current_node->offset.end && next_node != nullptr) {
                    if (column < next_node->offset.start) {
                        result.data = next_node;
                        result.offset = next_node->offset;
                        result.is_first = next_node->prev == nullptr;
                        result.is_final = next_node->next == nullptr;
                        result.type = piece_find_result_t::types::splice;
                        break;
                    }
                }
            }

            if (current_node == line->tail)
                break;

            current_node = current_node->next;
        }

        return result;
    }

    void piece_list::insert_after(
            piece_node_t* node,
            const piece_node_shared_ptr& piece,
            piece_table_undo_manager* undo_manager) {
        auto cloned_node = clone_and_swap(node, undo_manager);

        _owned_pieces.insert(piece);

        const auto piece_raw_ptr = piece.get();

        auto line = get_line(cloned_node->line);
        if (line->tail == cloned_node)
            line->tail = piece_raw_ptr;

        auto node_next = cloned_node->next;

        piece_raw_ptr->next = node_next;
        piece_raw_ptr->prev = cloned_node;
        if (node_next != nullptr)
            node_next->prev = piece_raw_ptr;
        else
            _tail = piece_raw_ptr;

        cloned_node->next = piece_raw_ptr;
    }

    void piece_list::insert_before(
            piece_node_t* node,
            const piece_node_shared_ptr& piece,
            piece_table_undo_manager* undo_manager) {
        auto cloned_node = clone_and_swap(node, undo_manager);

        _owned_pieces.insert(piece);

        const auto piece_raw_ptr = piece.get();
        auto node_prev = cloned_node->prev;

        auto line = get_line(cloned_node->line);
        if (line->head == cloned_node)
            line->head = piece_raw_ptr;

        piece_raw_ptr->next = cloned_node;
        piece_raw_ptr->prev = node_prev;
        if (node_prev != nullptr)
            node_prev->next = piece_raw_ptr;
        else
            _head = piece_raw_ptr;

        cloned_node->prev = piece_raw_ptr;
    }

    piece_line_t* piece_list::get_line(uint32_t line) {
        if (line >= _lines.size())
            adjust_lines_to_fit(line);
        return &_lines[line];
    }

    void piece_list::adjust_lines_to_fit(uint32_t line) {
        auto missing_line_count = std::max<size_t>(0, (line - _lines.size()) + 1);
        for (auto i = 0; i < missing_line_count; i++) {
            _lines.push_back({});
        }
    }

    ///////////////////////////////////////////////////////////////////////////

    void piece_table_undo_manager::clear() {
        clear_stack(_undo);
        clear_stack(_redo);
    }

    uint32_t piece_table_undo_manager::swap_node(
            piece_list& pieces,
            piece_node_t* node,
            piece_node_stack& target_stack) {
        auto line = pieces.get_line(node->line);
        auto original_head = line->head;

        uint32_t length = 0;

        if (node->prev != nullptr) {
            auto prev_node = node->prev;
            length = prev_node->next->length - node->length;
            target_stack.push(prev_node);
            prev_node->next = node;
        } else {
            target_stack.push(line->head);
            length = line->head->length - node->length;
            line->head = node;
            if (node->line == 0)
                pieces._head = node;
        }

        if (node->next != nullptr) {
            auto next_node = node->next;
            target_stack.push(next_node);
            next_node->prev = node;
        } else {
            if (line->tail != original_head)
                target_stack.push(line->tail);
            line->tail = node;
            if (node->line == pieces._lines.size() - 1)
                pieces._tail = node;
        }

        return length;
    }

    void piece_table_undo_manager::swap_deleted_node(
            piece_list& pieces,
            piece_node_t* node) {
        _undo.push(node);

        auto line = pieces.get_line(node->line);
        if (node->prev != nullptr) {
            node->prev->next = node->next;
        } else {
            line->head = node->next;
            if (node->line == 0)
                pieces._head = node->next;
        }

        auto current_node_prev = node->prev;
        if (node->next != nullptr) {
            node->next->prev = current_node_prev;
        } else {
            line->tail = current_node_prev;
            if (node->line == pieces._lines.size() - 1)
                pieces._tail = node;
        }
    }

    uint32_t piece_table_undo_manager::redo(piece_list& pieces) {
        if (_redo.empty())
            return 0;

        auto node = _redo.top();
        if (node == nullptr) {
            pieces.clear();
            _redo.pop();
            return 0;
        }

        auto length = swap_node(pieces, node, _undo);

        _redo.pop();

        return length;
    }

    uint32_t piece_table_undo_manager::undo(piece_list& pieces) {
        if (_undo.empty())
            return 0;

        auto node = _undo.top();
        if (node == nullptr) {
            pieces.clear();
            _undo.pop();
            return 0;
        }

        auto length = swap_node(pieces, node, _redo);

        _undo.pop();

        return length;
    }

    void piece_table_undo_manager::push_undo(piece_node_t* node) {
        _undo.push(node);
    }

    void piece_table_undo_manager::push_redo(piece_node_t* node) {
        _redo.push(node);
    }

    void piece_table_undo_manager::clear_stack(piece_node_stack& stack) {
        while (!stack.empty())
            stack.pop();
    }

};