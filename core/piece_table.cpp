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

    void piece_table::undo() {
        if (_undo_manager != nullptr)
            _undo_manager->undo(_pieces);
    }

    void piece_table::redo() {
        if (_undo_manager != nullptr)
            _undo_manager->redo(_pieces);
    }

    void piece_table::clear() {
        _pieces.clear();
        _changes.clear();
        _original.clear();
        if (_undo_manager != nullptr)
            _undo_manager->clear();
    }

    void piece_table::checkpoint() {
        if (_undo_manager != nullptr)
            _undo_manager->clear();
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
                    &_original);
            _pieces.add_tail(new_piece, _undo_manager);
        }
    }

    void piece_table::delete_at(uint32_t row, uint32_t column, uint32_t length) {
        if (length == 0)
            return;

        auto line = _pieces.get_line(row);

        auto initial_piece_find_result = _pieces.find_for_offset(row, column);
        auto final_piece_find_result = _pieces.find_for_offset(row, column + length);

        if (initial_piece_find_result.data == final_piece_find_result.data) {
            if (initial_piece_find_result.offset.start == column) {
                auto cloned_node = _pieces.clone_and_swap(
                        initial_piece_find_result.data,
                        _undo_manager);
                cloned_node->buffer_start += length;
                cloned_node->length -= length;
            } else if (final_piece_find_result.offset.end == column) {
                auto cloned_node = _pieces.clone_and_swap(
                        initial_piece_find_result.data,
                        _undo_manager);
                cloned_node->length -= length;
            } else {
                auto new_start = (initial_piece_find_result.data->buffer_start
                                 + (column - initial_piece_find_result.data->buffer_start)
                                 + length);

                auto new_piece = std::make_shared<piece_node_t>(
                        row,
                        new_start,
                        initial_piece_find_result.data->length - new_start,
                        &_changes);

                auto cloned_piece = _pieces.clone_and_swap(
                        initial_piece_find_result.data,
                        _undo_manager);
                cloned_piece->length = column;

                _pieces.insert_after(cloned_piece, new_piece);
            }
        } else {
            uint32_t start = column;
            uint32_t end = column + length;

            auto current_node = line->head;

            while (current_node != nullptr) {
                if (current_node->offset.start < start
                &&  current_node->offset.end < start)
                    goto next_node;

                if (current_node->offset.start > end)
                    break;

                if (start <= current_node->offset.start
                &&  end >= current_node->offset.end) {
                    if (_undo_manager != nullptr)
                        _undo_manager->swap_deleted_node(_pieces, current_node);
                } else if (end <= current_node->offset.end) {
                    auto element_removed_count = end - current_node->offset.start;
                    auto cloned_node = _pieces.clone_and_swap(current_node, _undo_manager);
                    cloned_node->buffer_start += element_removed_count;
                    cloned_node->length -= element_removed_count;
                } else {
                    auto cloned_node = _pieces.clone_and_swap(current_node, _undo_manager);
                    cloned_node->length -= cloned_node->offset.end - start;
                }

            next_node:
                if (current_node == line->tail)
                    break;

                current_node = current_node->next;
            }
        }

        _pieces.update_offsets();
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

    void piece_table::insert_at(uint32_t row, uint32_t column, const element_list_t& elements) {
        if (elements.empty())
            return;

        for (const auto& element : elements)
            _changes.elements.push_back(element);

        auto new_change_offset = _changes.size() - elements.size();
        auto find_result = _pieces.find_for_offset(row, column);
        auto is_change_piece = find_result.data != nullptr ?
                               find_result.data->buffer->type == piece_table_buffer_t::changes :
                               false;
        switch (find_result.type) {
            case piece_find_result_t::none: {
                auto node = std::make_shared<piece_node_t>(
                        row,
                        new_change_offset,
                        elements.size(),
                        &_changes);
                node->offset = offset_t {column, static_cast<uint32_t>(column + elements.size())};
                _pieces.add_tail(node, _undo_manager);
                break;
            }
            case piece_find_result_t::first: {
                auto buffer_end = find_result.data->buffer_end();
                if (is_change_piece && buffer_end == new_change_offset) {
                    auto cloned_node = _pieces.clone_and_swap(find_result.data, _undo_manager);
                    cloned_node->length++;
                } else {
                    auto node = std::make_shared<piece_node_t>(
                            row,
                            new_change_offset,
                            elements.size(),
                            &_changes);
                    node->offset = offset_t {column, static_cast<uint32_t>(column + elements.size())};
                    _pieces.insert_head(node, _undo_manager);
                }
                break;
            }
            case piece_find_result_t::final: {
                auto buffer_end = find_result.data->buffer_end();
                if (is_change_piece && buffer_end == new_change_offset) {
                    auto cloned_node = _pieces.clone_and_swap(find_result.data, _undo_manager);
                    cloned_node->length++;
                } else {
                    auto node = std::make_shared<piece_node_t>(
                            row,
                            new_change_offset,
                            elements.size(),
                            &_changes);
                    node->offset = offset_t {column, static_cast<uint32_t>(column + elements.size())};
                    _pieces.add_tail(node, _undo_manager);
                }
                break;
            }
            case piece_find_result_t::medial: {
                if (new_change_offset == find_result.data->buffer_end()
                &&  find_result.offset.end == column) {
                    auto cloned_node = _pieces.clone_and_swap(find_result.data, _undo_manager);
                    cloned_node->length++;
                    break;
                }

                auto new_piece = std::make_shared<piece_node_t>(
                        row,
                        static_cast<uint32_t>(new_change_offset),
                        elements.size(),
                        &_changes);

                auto left_piece = _pieces.clone_and_swap(
                        find_result.data,
                        _undo_manager);

                int32_t right_piece_length = std::max<int32_t>(
                        static_cast<int32_t>(left_piece->offset.end - (column + 1)),
                        0);
                auto right_piece = std::make_shared<piece_node_t>(
                        row,
                        column + 1,
                        right_piece_length,
                        left_piece->buffer
                );

                int32_t left_piece_length = std::max<int32_t>(
                        static_cast<int32_t>(left_piece->offset.end - column),
                        0);
                left_piece->length -= left_piece_length;

                _pieces.insert_after(left_piece, new_piece);
                _pieces.insert_after(new_piece.get(), right_piece);
                break;
            }
            default:
                // XXX: should never happen
                break;
        }

        _pieces.update_offsets();
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

    attr_span_list_t piece_table::sub_sequence(uint32_t row, uint32_t start, uint32_t end) {
        attr_span_list_t spans;

        auto line = _pieces.get_line(row);
        auto current_node = line->head;

        while (current_node != nullptr) {
            if (current_node->offset.start < start && current_node->offset.end < start) goto next_node;
            if (current_node->offset.start > end) break;

            if (end <= current_node->offset.end) {
                if (current_node->offset.within(start)) {
                    current_node->copy_elements(
                            spans,
                            start,
                            end - current_node->offset.start);
                } else {
                    current_node->copy_elements(
                            spans,
                            0,
                            end - current_node->offset.start);
                }
            } else if (start <= current_node->offset.start) {
                current_node->copy_elements(spans);
            } else {
                auto start_offset = start - current_node->buffer_start;
                current_node->copy_elements(
                        spans,
                        start_offset,
                        current_node->buffer_start + current_node->length);
            }

        next_node:
            if (current_node == line->tail)
                break;

            current_node = current_node->next;
        }

        return spans;
    }

    void piece_table::paste(const selection_t& selection, const element_list_t& elements) {
        delete_at(selection.line, selection.start, selection.length);
        insert_at(selection.line, selection.start, elements);
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

    void piece_list::update_offsets() {
        for (const auto& line : _lines) {
            uint32_t offset = 0;
            auto current_node = line.head;
            while (current_node != nullptr) {
                current_node->offset = offset_t {
                        offset,
                        offset + current_node->length
                };
                offset += current_node->length;

                if (current_node == line.tail)
                    break;

                current_node = current_node->next;
            }
        }
    }

    void piece_list::add_tail(
            const piece_node_shared_ptr& piece,
            piece_table_undo_manager* undo_manager) {
        auto raw_ptr = piece.get();
        _owned_pieces.insert(piece);
        if (_head == nullptr) {
            if (undo_manager != nullptr)
                undo_manager->push_undo(nullptr);
            _head = raw_ptr;
            _tail = raw_ptr;
        } else {
            if (undo_manager != nullptr)
                undo_manager->push_undo(_tail);
            raw_ptr->prev = _tail;
            _tail->next = raw_ptr;
            _tail = raw_ptr;
        }

        auto line = get_line(piece->line);

        if (line->head == nullptr) {
            line->head = raw_ptr;
            line->tail = raw_ptr;
        } else {
            line->tail = raw_ptr;
        }
    }

    void piece_list::insert_head(
            const piece_node_shared_ptr& piece,
            piece_table_undo_manager* undo_manager) {
        auto raw_ptr = piece.get();
        _owned_pieces.insert(piece);
        if (_head == nullptr) {
            if (undo_manager != nullptr)
                undo_manager->push_undo(nullptr);
            _head = raw_ptr;
            _tail = raw_ptr;
        } else {
            if (undo_manager != nullptr)
                undo_manager->push_undo(_tail);
            raw_ptr->next = _head;
            _head->prev = raw_ptr;
            _head = raw_ptr;
        }

        auto line = get_line(piece->line);

        if (line->head == nullptr) {
            line->head = raw_ptr;
            line->tail = raw_ptr;
        } else {
            line->head = raw_ptr;
        }
    }

    piece_node_t* piece_list::clone_and_swap(
            piece_node_t* node,
            piece_table_undo_manager* undo_manager) {
        auto clone_node = std::make_shared<piece_node_t>(
                node->line,
                node->buffer_start,
                node->length,
                node->buffer);
        clone_node->offset = node->offset;

        _owned_pieces.insert(clone_node);

        auto clone_raw_ptr = clone_node.get();
        if (node->prev != nullptr) {
            node->prev->next = clone_raw_ptr;
            clone_raw_ptr->prev = node->prev;
        } else {
            _head = clone_raw_ptr;
        }
        if (node->next != nullptr) {
            node->next->prev = clone_raw_ptr;
            clone_raw_ptr->next = node->next;
        } else {
            _tail = clone_raw_ptr;
        }

        if (undo_manager != nullptr)
            undo_manager->push_undo(node);

        auto line = get_line(node->line);
        if (line->head == node)
            line->head = clone_raw_ptr;
        if (line->tail == node)
            line->tail = clone_raw_ptr;

        return clone_raw_ptr;
    }

    piece_find_result_t piece_list::find_for_offset(uint32_t row, uint32_t column) {
        piece_find_result_t result {};
        if (empty()) {
            return result;
        }

        auto line = get_line(row);

        if (column == 0) {
            result.data = line->head;
            if (result.data != nullptr) {
                result.type = piece_find_result_t::types::first;
                result.offset = result.data->offset;
            }
        } else if (column >= line->total_length()) {
            result.data = line->tail;
            if (result.data != nullptr) {
                result.type = piece_find_result_t::types::final;
                result.offset = result.data->offset;
            }
        } else {
            auto current_node = line->head;
            while (current_node != nullptr) {
                if (current_node->offset.within(column)) {
                    break;
                }
                if (current_node == line->tail)
                    break;
                current_node = current_node->next;
            }
            result.data = current_node;
            result.type = piece_find_result_t::types::medial;
            if (current_node != nullptr)
                result.offset = current_node->offset;
        }

        return result;
    }

    void piece_list::insert_after(piece_node_t* node, const piece_node_shared_ptr& piece) {
        _owned_pieces.insert(piece);

        const auto piece_raw_ptr = piece.get();

        auto line = get_line(node->line);
        if (line->tail == node)
            line->tail = piece_raw_ptr;

        auto node_next = node->next;

        piece_raw_ptr->next = node_next;
        piece_raw_ptr->prev = node;
        if (node_next != nullptr)
            node_next->prev = piece_raw_ptr;
        else
            _tail = piece_raw_ptr;

        node->next = piece_raw_ptr;
    }

    void piece_list::insert_before(piece_node_t* node, const piece_node_shared_ptr& piece) {
        _owned_pieces.insert(piece);

        const auto piece_raw_ptr = piece.get();
        auto node_prev = node->prev;

        // XXX: not handling line->head/line->tail!

        piece_raw_ptr->next = node;
        piece_raw_ptr->prev = node_prev;
        if (node_prev != nullptr)
            node_prev->next = piece_raw_ptr;
        else
            _head = piece_raw_ptr;

        node->prev = piece_raw_ptr;
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

    void piece_table_undo_manager::swap_node(
            piece_list& pieces,
            piece_node_t* node,
            piece_node_stack& target_stack) {
        if (node->prev == nullptr) {
            auto current_head = pieces._head;
            target_stack.push(current_head);
            pieces._head = node;
            if (current_head == pieces._tail) {
                pieces._tail = pieces._head;
            }
        } else if (node->next == nullptr) {
            auto current_tail = pieces._tail;
            target_stack.push(current_tail);
            pieces._tail = node;
            if (current_tail == pieces._head) {
                pieces._head = pieces._tail;
            }
        }

        auto line = pieces.get_line(node->line);
        if (node->prev != nullptr) {
            auto prev_node = node->prev;
            if (prev_node->next == line->head)
                line->head = node;
            prev_node->next = node;
        }

        if (node->next != nullptr) {
            auto next_node = node->next;
            if (next_node->prev == line->tail)
                line->tail = node;
            next_node->prev = node;
        }
    }

    void piece_table_undo_manager::swap_deleted_node(
            piece_list& pieces,
            piece_node_t* node) {
        _undo.push(node);

        if (node->prev == nullptr) {
            auto current_head = pieces._head;
            pieces._head = node;
            if (current_head == pieces._tail) {
                pieces._tail = pieces._head;
            }
        } else if (node->next == nullptr) {
            auto current_tail = pieces._tail;
            pieces._tail = node;
            if (current_tail == pieces._head) {
                pieces._head = pieces._tail;
            }
        }

        auto line = pieces.get_line(node->line);
        auto current_node_prev = node->prev;
        if (node->prev != nullptr) {
            node->prev->next = node->next;
        }

        if (node->next != nullptr) {
            node->next->prev = current_node_prev;
        }

        if (line->tail == node)
            line->tail = node->next;

        if (line->head == node)
            line->head = current_node_prev;
    }

    void piece_table_undo_manager::undo(piece_list& pieces) {
        if (_undo.empty())
            return;

        auto node = _undo.top();
        if (node == nullptr) {
            pieces.clear();
            _undo.pop();
            return;
        }

        swap_node(pieces, node, _redo);

        _undo.pop();
    }

    void piece_table_undo_manager::redo(piece_list& pieces) {
        if (_redo.empty())
            return;

        auto node = _redo.top();
        if (node == nullptr) {
            pieces.clear();
            _redo.pop();
            return;
        }

        swap_node(pieces, node, _undo);

        _redo.pop();
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