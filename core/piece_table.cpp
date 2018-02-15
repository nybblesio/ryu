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
#include <algorithm>
#include "piece_table.h"

namespace ryu::core {

    void piece_node_t::copy_elements(
            attr_line_list& lines,
            uint32_t begin,
            uint32_t end) {
        attr_span_list_t* current_line = nullptr;
        attr_span_t* current_span = nullptr;
        if (!lines.empty()) {
            current_line = &lines.back();
            if (!current_line->empty())
                current_span = &current_line->back();
        } else {
            lines.push_back(attr_span_list_t {});
            current_line = &lines.back();
        }

        std::stringstream stream {};
        for (size_t i = begin; i < end; i++) {
            auto& element = buffer->elements[start + i];
            if (current_span != nullptr) {
                if (current_span->attr != element.attr) {
                    current_span->text += stream.str();
                    stream.str("");
                    current_line->push_back(attr_span_t {element.attr});
                    current_span = &current_line->back();
                }
            } else {
                current_line->push_back(attr_span_t {element.attr});
                current_span = &current_line->back();
            }
            if (element.safe_value(stream)) {
                current_span->text += stream.str();
                stream.str("");
                lines.push_back(attr_span_list_t {});
                current_line = &lines.back();
                current_span = nullptr;
            }
        }

        if (current_span != nullptr)
            current_span->text += stream.str();
    }

    void piece_node_t::copy_elements(attr_line_list& lines) {
        copy_elements(lines, 0, length);
    }

    ///////////////////////////////////////////////////////////////////////////

    void piece_table_t::undo() {
        _pieces.undo();
    }

    void piece_table_t::redo() {
        _pieces.redo();
    }

    void piece_table_t::clear() {
        _pieces.clear();
        _changes.clear();
        _original.clear();
    }

    void piece_table_t::checkpoint() {
        _pieces.checkpoint();
    }

    const selection_t& piece_table_t::add_selection(
            selection_t::types type,
            uint32_t start,
            uint32_t length,
            const std::string& name) {
        _selections.push_back(selection_t{name, start, length, type});
        return _selections.back();
    }

    const attr_line_list& piece_table_t::sequence() {
        if (!_lines.empty())
            return _lines;
        auto current_node = _pieces.head;
        while (current_node != nullptr) {
            current_node->copy_elements(_lines);
            current_node = current_node->next;
        }
        return _lines;
    }

    // TODO: can we consolidate this into piece_list_t?
    void piece_table_t::swap_deleted_node(piece_node_t* node) {
        _pieces.undo_stack.push(node);

        if (node->prev == nullptr) {
            auto current_head = _pieces.head;
            _pieces.head = node;
            if (current_head == _pieces.tail) {
                _pieces.tail = _pieces.head;
            }
        } else if (node->next == nullptr) {
            auto current_tail = _pieces.tail;
            _pieces.tail = node;
            if (current_tail == _pieces.head) {
                _pieces.head = _pieces.tail;
            }
        }

        auto current_node_prev = node->prev;
        if (node->prev != nullptr) {
            node->prev->next = node->next;
        }
        if (node->next != nullptr) {
            node->next->prev = current_node_prev;
        }
    }

    void piece_table_t::load(const piece_table_buffer_t& buffer) {
        clear();
        _original = buffer;
        _lines.clear();

        if (!buffer.empty()) {
            _pieces.add_tail(std::make_shared<piece_node_t>(
                    0,
                    _original.elements.size(),
                    &_original));
        }
    }

    void piece_table_t::delete_at(uint32_t offset, uint32_t length) {
        if (length == 0)
            return;

        _lines.clear();

        auto initial_piece_find_result = _pieces.find_for_offset(offset);
        auto final_piece_find_result = _pieces.find_for_offset(offset + length);

        if (initial_piece_find_result.data == final_piece_find_result.data) {
            if (initial_piece_find_result.linear_offset - initial_piece_find_result.data->length == offset) {
                auto cloned_node = _pieces.clone_and_swap(initial_piece_find_result.data);
                cloned_node->start += length;
                cloned_node->length -= length;
            } else if (final_piece_find_result.linear_offset == offset) {
                auto cloned_node = _pieces.clone_and_swap(initial_piece_find_result.data);
                cloned_node->length -= length;
            } else {
                auto new_start = (initial_piece_find_result.data->start
                                 + (offset - initial_piece_find_result.data->start)
                                 + length);

                auto new_piece = std::make_shared<piece_node_t>(
                        new_start,
                        initial_piece_find_result.data->length - new_start,
                        &_changes);

                auto cloned_piece = _pieces.clone_and_swap(initial_piece_find_result.data);
                cloned_piece->length = offset;

                _pieces.insert_after(cloned_piece, new_piece);
            }
        } else {
            uint32_t node_start_offset = 0;
            uint32_t delete_offset_start = offset;
            uint32_t delete_offset_end = offset + length;
            auto current_node = _pieces.head;
            while (current_node != nullptr
                && delete_offset_start < delete_offset_end) {

                auto node_end_offset = node_start_offset + current_node->length;
                auto elements_remaining = delete_offset_start - node_start_offset;

                if (!(delete_offset_start >= node_start_offset
                   && delete_offset_start <= node_end_offset)) {
                    goto next_node;
                }

                if (delete_offset_end < node_end_offset) {
                    auto element_removed_count = delete_offset_end - node_start_offset;
                    auto cloned_node = _pieces.clone_and_swap(current_node);
                    cloned_node->start += element_removed_count;
                    cloned_node->length -= element_removed_count;
                    goto next_node;
                }

                if (elements_remaining == 0) {
                    swap_deleted_node(current_node);
                } else {
                    auto cloned_node = _pieces.clone_and_swap(current_node);
                    cloned_node->length -= cloned_node->length - elements_remaining;
                }

            next_node:
                node_start_offset += current_node->length;
                delete_offset_start = node_start_offset;
                current_node = current_node->next;
            }
        }
    }

    void piece_table_t::remove_selection(const selection_t& selection) {
        _selections.erase(std::remove(
                _selections.begin(),
                _selections.end(),
                selection),
            _selections.end());
    }

    void piece_table_t::insert_at(uint32_t offset, const element_t& element) {
        _lines.clear();
        _changes.elements.push_back(element);

        auto new_change_offset = _changes.size() - 1;
        auto find_result = _pieces.find_for_offset(offset);
        auto is_change_piece = find_result.data != nullptr ?
                               find_result.data->buffer->type == piece_table_buffer_t::changes :
                               false;
        switch (find_result.type) {
            case piece_find_result_t::none: {
                _pieces.add_tail(std::make_shared<piece_node_t>(new_change_offset, 1, &_changes));
                break;
            }
            case piece_find_result_t::first: {
                if (is_change_piece && find_result.data->end() + 1 == offset) {
                    auto cloned_node = _pieces.clone_and_swap(find_result.data);
                    cloned_node->length++;
                } else {
                    _pieces.add_tail(std::make_shared<piece_node_t>(new_change_offset, 1, &_changes));
                }
                break;
            }
            case piece_find_result_t::final: {
                if (is_change_piece) {
                    auto cloned_node = _pieces.clone_and_swap(find_result.data);
                    cloned_node->length++;
                } else {
                    _pieces.add_tail(std::make_shared<piece_node_t>(new_change_offset, 1, &_changes));
                }
                break;
            }
            case piece_find_result_t::medial: {
                if (new_change_offset == find_result.data->end()
                &&  find_result.linear_offset == offset) {
                    auto cloned_node = _pieces.clone_and_swap(find_result.data);
                    cloned_node->length++;
                    break;
                }

                auto new_piece = std::make_shared<piece_node_t>(
                        static_cast<uint32_t>(new_change_offset),
                        1,
                        &_changes);

                auto left_piece = _pieces.clone_and_swap(find_result.data);

                int32_t right_piece_length = std::max<int32_t>(left_piece->length - (offset + 1), 0);
                auto right_piece = std::make_shared<piece_node_t>(
                        offset + 1,
                        right_piece_length,
                        left_piece->buffer
                );
                left_piece->length = offset;

                _pieces.insert_after(left_piece, new_piece);
                _pieces.insert_after(new_piece.get(), right_piece);
                break;
            }
            default:
                // XXX: should never happen
                break;
        }
    }

    attr_line_list piece_table_t::cut(const selection_t& selection) {
        auto lines = copy(selection);
        delete_selection(selection);
        return lines;
    }

    attr_line_list piece_table_t::copy(const selection_t& selection) {
        return sub_sequence(selection.start, selection.start + selection.length);
    }

    void piece_table_t::delete_selection(const selection_t& selection) {
        delete_at(selection.start, selection.length);
    }

    attr_line_list piece_table_t::sub_sequence(uint32_t start, uint32_t end) {
        attr_line_list lines;
        uint32_t node_start_offset = 0;
        auto current_node = _pieces.head;
        while (current_node != nullptr) {
            auto node_end_offset = node_start_offset + current_node->length;
            if (node_start_offset < start && node_end_offset < start) goto next_node;
            if (node_start_offset > end) break;

            if (end <= node_end_offset) {
                if (start >= node_start_offset && start <= node_end_offset) {
                    current_node->copy_elements(
                            lines,
                            start,
                            end - node_start_offset);
                } else {
                    current_node->copy_elements(
                            lines,
                            0,
                            end - node_start_offset);
                }
            } else if (start <= node_start_offset) {
                current_node->copy_elements(lines);
            } else {
                auto start_offset = start - current_node->start;
                current_node->copy_elements(
                        lines,
                        start_offset,
                        current_node->start + current_node->length);
            }
        next_node:
            node_start_offset += current_node->length;
            current_node = current_node->next;
        }
        return lines;
    }

    void piece_table_t::paste(const selection_t& selection, const element_list& elements) {
        delete_at(selection.start, selection.length);
        auto offset = 0;
        for (const auto& element : elements)
            insert_at(selection.start + offset++, element);
    }

    const selection_list& piece_table_t::selections() const {
        return _selections;
    }

    ///////////////////////////////////////////////////////////////////////////

    void piece_list_t::undo() {
        if (undo_stack.empty())
            return;

        auto node = undo_stack.top();
        if (node == nullptr) {
            head = nullptr;
            tail = nullptr;
            undo_stack.pop();
            return;
        }

        swap_node(node, redo_stack);

        undo_stack.pop();
    }

    void piece_list_t::redo() {
        if (redo_stack.empty())
            return;

        auto node = redo_stack.top();
        if (node == nullptr) {
            head = nullptr;
            tail = nullptr;
            redo_stack.pop();
            return;
        }

        swap_node(node, undo_stack);

        redo_stack.pop();
    }

    void piece_list_t::clear() {
        head = nullptr;
        tail = nullptr;
        while (!undo_stack.empty())
            undo_stack.pop();
        while (!redo_stack.empty())
            redo_stack.pop();
        owned_pieces.clear();
    }

    void piece_list_t::checkpoint() {
        clear_stack(undo_stack);
        clear_stack(redo_stack);
    }

    size_t piece_list_t::size() const {
        size_t count = 0;
        auto current_node = head;
        while (current_node != nullptr) {
            ++count;
            current_node = current_node->next;
        }
        return count;
    }

    uint32_t piece_list_t::total_length() const {
        uint32_t length = 0;
        auto current_node = head;
        while (current_node != nullptr) {
            length += current_node->length;
            current_node = current_node->next;
        }
        return length;
    }

    void piece_list_t::swap_node(
            piece_node_t* node,
            piece_node_stack& target_stack) {
        if (node->prev == nullptr) {
            auto current_head = head;
            target_stack.push(current_head);
            head = node;
            if (current_head == tail) {
                tail = head;
            }
        } else if (node->next == nullptr) {
            auto current_tail = tail;
            target_stack.push(current_tail);
            tail = node;
            if (current_tail == head) {
                head = tail;
            }
        }

        if (node->prev != nullptr) {
            auto prev_node = node->prev;
            prev_node->next = node;
        }

        if (node->next != nullptr) {
            auto next_node = node->next;
            next_node->prev = node;
        }
    }

    void piece_list_t::clear_stack(piece_node_stack& stack) {
        while (!stack.empty()) {
            auto top = stack.top();
            auto it = owned_pieces.begin();
            for (; it != owned_pieces.end(); ++it) {
                if ((*it).get() == top) {
                    owned_pieces.erase(it);
                    break;
                }
            }
            stack.pop();
        }
    }

    void piece_list_t::add_tail(const piece_node_shared_ptr& piece) {
        auto raw_ptr = piece.get();
        owned_pieces.insert(piece);
        if (head == nullptr) {
            undo_stack.push(nullptr);
            head = raw_ptr;
            tail = raw_ptr;
        } else {
            undo_stack.push(tail);
            raw_ptr->prev = tail;
            tail->next = raw_ptr;
            tail = raw_ptr;
        }
    }

    void piece_list_t::insert_head(const piece_node_shared_ptr& piece) {
        auto raw_ptr = piece.get();
        owned_pieces.insert(piece);
        if (head == nullptr) {
            undo_stack.push(nullptr);
            head = raw_ptr;
            tail = raw_ptr;
        } else {
            undo_stack.push(head);
            raw_ptr->next = head;
            head->prev = raw_ptr;
            head = raw_ptr;
        }
    }

    piece_node_t* piece_list_t::clone_and_swap(piece_node_t* node) {
        auto clone_node = std::make_shared<piece_node_t>(
                node->start,
                node->length,
                node->buffer);
        owned_pieces.insert(clone_node);
        auto clone_raw_ptr = clone_node.get();
        if (node->prev != nullptr) {
            node->prev->next = clone_raw_ptr;
            clone_raw_ptr->prev = node->prev;
        } else {
            head = clone_raw_ptr;
        }
        if (node->next != nullptr) {
            node->next->prev = clone_raw_ptr;
            clone_raw_ptr->next = node->next;
        } else {
            tail = clone_raw_ptr;
        }
        undo_stack.push(node);
        return clone_raw_ptr;
    }

    piece_find_result_t piece_list_t::find_for_offset(uint32_t offset) {
        piece_find_result_t result {};
        if (empty()) {
            return result;
        }

        auto total_linear_length = total_length();

        if (offset == 0) {
            result.data = head;
            result.type = piece_find_result_t::types::first;
            result.linear_offset = result.data != nullptr ? result.data->length : 0;
        } else if (offset >= total_linear_length) {
            result.data = tail;
            result.type = piece_find_result_t::types::final;
            auto final_linear_offset = total_linear_length;
            if (result.data != nullptr)
                final_linear_offset = linear_offset(result.data);
            result.linear_offset = final_linear_offset;
        } else {
            uint32_t piece_offset = 0;
            auto current_node = head;
            while (current_node != nullptr) {
                if (offset >= piece_offset
                &&  offset <= piece_offset + current_node->length) {
                    break;
                }
                piece_offset += current_node->length;
                current_node = current_node->next;
            }
            result.data = current_node;
            result.type = piece_find_result_t::types::medial;
            result.linear_offset = current_node != nullptr ? piece_offset + current_node->length : 0;
        }

        return result;
    }

    uint32_t piece_list_t::linear_offset(const piece_node_t* start_node) const {
        uint32_t offset = 0;
        auto current_node = start_node;
        while (current_node != nullptr) {
            offset += current_node->length;
            current_node = current_node->prev;
        }
        return offset;
    }

    void piece_list_t::insert_after(piece_node_t* node, const piece_node_shared_ptr& piece) {
        owned_pieces.insert(piece);

        const auto piece_raw_ptr = piece.get();
        auto node_next = node->next;

        piece_raw_ptr->next = node_next;
        piece_raw_ptr->prev = node;
        if (node_next != nullptr)
            node_next->prev = piece_raw_ptr;
        else
            tail = piece_raw_ptr;

        node->next = piece_raw_ptr;
    }

    void piece_list_t::insert_before(piece_node_t* node, const piece_node_shared_ptr& piece) {
        owned_pieces.insert(piece);

        const auto piece_raw_ptr = piece.get();
        auto node_prev = node->prev;

        piece_raw_ptr->next = node;
        piece_raw_ptr->prev = node_prev;
        if (node_prev != nullptr)
            node_prev->next = piece_raw_ptr;
        else
            head = piece_raw_ptr;

        node->prev = piece_raw_ptr;
    }

};