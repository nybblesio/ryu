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

    ///////////////////////////////////////////////////////////////////////////

    bool element_t::safe_value(std::stringstream& stream) const {
        auto newline = false;
        if (is_newline())
            newline = true;
        else if (is_tab())
            stream << "    ";           // XXX: this is hard coded
        else if (is_percent())
            stream << "%%";
        else if (is_space())
            stream << " ";
        else
            stream << value;
        return newline;
    }

    ///////////////////////////////////////////////////////////////////////////

    void piece_node_t::copy_elements(attr_line_list& lines) {
        if (length == 0)
            return;

        attr_span_list* current_line = nullptr;
        attr_span_t* current_span = nullptr;
        if (!lines.empty()) {
            current_line = &lines.back();
            if (!current_line->empty())
                current_span = &current_line->back();
        } else {
            lines.push_back({});
            current_line = &lines.back();
        }

        std::stringstream stream {};
        for (size_t i = 0; i < length; i++) {
            auto& element = buffer->elements[start + i];
            if (current_span != nullptr) {
                if (current_span->attr != element.attr) {
                    current_span->text += stream.str();
                    stream.str("");
                    current_line->push_back({element.attr});
                    current_span = &current_line->back();
                }
            } else {
                current_line->push_back({element.attr});
                current_span = &current_line->back();
            }
            if (element.safe_value(stream)) {
                lines.push_back({});
                current_line = &lines.back();
            }
        }

        if (current_span != nullptr)
            current_span->text += stream.str();
    }

    ///////////////////////////////////////////////////////////////////////////

    void piece_table_t::undo() {
        pieces.undo();
    }

    void piece_table_t::redo() {
        pieces.redo();
    }

    void piece_table_t::clear() {
        pieces.clear();
        changes.clear();
        original.clear();
    }

    const selection_t& piece_table_t::add_selection(
            selection_t::types type,
            uint32_t start,
            uint32_t length,
            const std::string& name) {
        selections.push_back(selection_t{name, start, length, type});
        return selections.back();
    }

    const attr_line_list& piece_table_t::sequence() {
        if (!lines.empty())
            return lines;
        auto current_node = pieces.head;
        while (current_node != nullptr) {
            current_node->copy_elements(lines);
            current_node = current_node->next;
        }
        return lines;
    }

    void piece_table_t::load(const piece_table_buffer_t& buffer) {
        clear();
        original = buffer;
        lines.clear();

        if (!buffer.empty()) {
            pieces.add_tail(std::make_shared<piece_node_t>(
                    0,
                    original.elements.size(),
                    &original));
        }
    }

    void piece_table_t::delete_at(uint32_t offset, uint32_t length) {
        if (length == 0)
            return;

        lines.clear();

        auto initial_piece_find_result = pieces.find_for_offset(offset);
        size_t initial_linear_offset = 0;
        if (initial_piece_find_result.type != piece_find_result_t::none) {
            initial_linear_offset = pieces.linear_offset(const_cast<piece_node_t*>(initial_piece_find_result.data));
        }

        auto final_piece_find_result = pieces.find_for_offset(static_cast<uint32_t>(offset + length));
        size_t final_linear_offset = 0;
        if (final_piece_find_result.type != piece_find_result_t::none) {
            final_linear_offset = pieces.linear_offset(const_cast<piece_node_t*>(initial_piece_find_result.data));
        }

        if (initial_piece_find_result.data == final_piece_find_result.data) {
            if (initial_linear_offset - initial_piece_find_result.data->length == offset) {
                auto cloned_node = pieces.clone_and_swap(initial_piece_find_result.data);
                cloned_node->start += length;
                cloned_node->length -= length;
            } else if (final_linear_offset == offset) {
                auto cloned_node = pieces.clone_and_swap(initial_piece_find_result.data);
                cloned_node->length -= length;
            } else {
                auto new_start = (initial_piece_find_result.data->start
                                 + (offset - initial_piece_find_result.data->start)
                                 + length);

                auto new_piece = std::make_shared<piece_node_t>(
                        new_start,
                        initial_piece_find_result.data->length - new_start,
                        &changes);

                auto cloned_piece = pieces.clone_and_swap(initial_piece_find_result.data);
                cloned_piece->length = offset;

                pieces.insert_after(cloned_piece, new_piece);
            }
        } else {
            //
            //  piece1 <-> piece2 ... piece30 <-> piece31
            //
            //     ^-----------------------------------^
            //                  delete
            //
            // 1. merge linked list nodes
            //      piece1 <-> piece31
            //
            // 2. adjust piece1 & piece31 start/length for internal delete
            //

            auto cloned_initial_node = pieces.clone_and_swap(initial_piece_find_result.data);
            auto cloned_final_node = pieces.clone_and_swap(final_piece_find_result.data);

            if (cloned_initial_node->next != cloned_final_node->prev) {
                cloned_initial_node->next = cloned_final_node;
                cloned_final_node->prev = cloned_initial_node;
            }

            // XXX: clamp to 0
            auto initial_offset = (cloned_initial_node->start
                              + (offset - cloned_initial_node->start)
                              + length);
            cloned_initial_node->length -= initial_offset;
            if (cloned_initial_node->length == 0) {
                // cut out node?
            }

            // XXX: clamp to 0
            auto final_offset = (cloned_final_node->start
                                   + ((offset + length) - cloned_final_node->start)
                                   + length);
            cloned_final_node->start += final_offset;
            cloned_final_node->length -= final_offset;
            if (cloned_final_node->length == 0) {
                // cut out node?
            }
        }
    }

    void piece_table_t::remove_selection(const selection_t& selection) {
        std::remove(selections.begin(), selections.end(), selection);
    }

    void piece_table_t::insert_at(uint32_t offset, const element_t& element) {
        lines.clear();
        changes.elements.push_back(element);

        auto find_result = pieces.find_for_offset(offset);
        auto is_change_piece = find_result.data != nullptr ?
                               find_result.data->buffer->type == piece_table_buffer_t::changes :
                               false;
        switch (find_result.type) {
            case piece_find_result_t::none: {
                pieces.add_tail(std::make_shared<piece_node_t>(offset, 1, &changes));
                break;
            }
            case piece_find_result_t::first: {
                if (is_change_piece && find_result.data->end() + 1 == offset) {
                    auto cloned_node = pieces.clone_and_swap(find_result.data);
                    cloned_node->length++;
                } else {
                    pieces.add_tail(std::make_shared<piece_node_t>(offset, 1, &changes));
                }
                break;
            }
            case piece_find_result_t::final: {
                if (is_change_piece) {
                    auto cloned_node = pieces.clone_and_swap(find_result.data);
                    cloned_node->length++;
                } else {
                    pieces.add_tail(std::make_shared<piece_node_t>(offset, 1, &changes));
                }
                break;
            }
            case piece_find_result_t::medial: {
                auto new_change_offset = changes.size() - 1;
                auto linear_offset = pieces.linear_offset(const_cast<piece_node_t*>(find_result.data));

                if (linear_offset == offset) {
                    find_result.data->length++;
                    break;
                }

                auto new_piece = std::make_shared<piece_node_t>(
                        static_cast<uint32_t>(new_change_offset),
                        1,
                        &changes);

                auto left_piece = pieces.clone_and_swap(find_result.data);
                auto right_piece = std::make_shared<piece_node_t>(
                        offset + 1,
                        left_piece->length - (offset + 1),
                        left_piece->buffer
                );
                left_piece->length = offset;

                pieces.insert_after(left_piece, new_piece);
                pieces.insert_after(new_piece.get(), right_piece);
                break;
            }
            default:
                // XXX: should never happen
                break;
        }
    }

    element_list piece_table_t::cut(const selection_t& selection) {
        auto elements = copy(selection);
        delete_selection(selection);
        return elements;
    }

    element_list piece_table_t::copy(const selection_t& selection) {
        element_list elements {};
        auto formatted_lines = sequence();
        return elements;
    }

    void piece_table_t::delete_selection(const selection_t& selection) {
        delete_at(selection.start, selection.length);
    }

    void piece_table_t::paste(const selection_t& selection, const element_list& elements) {
        delete_at(selection.start, selection.length);
        auto offset = 0;
        for (const auto& element : elements)
            insert_at(selection.start + offset++, element);
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

    size_t piece_list_t::size() const {
        size_t count = 0;
        auto current_node = head;
        while (current_node != nullptr) {
            ++count;
            current_node = current_node->next;
        }
        return count;
    }

    size_t piece_list_t::total_length() const {
        size_t length = 0;
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

    void piece_list_t::add_tail(const piece_shared_ptr& piece) {
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

    void piece_list_t::insert_head(const piece_shared_ptr& piece) {
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

        if (offset == 0) {
            result.data = head;
            result.type = piece_find_result_t::types::first;
        } else if (offset >= total_length()) {
            result.data = tail;
            result.type = piece_find_result_t::types::final;
        } else {
            auto piece_offset = 0;
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
        }

        return result;
    }

    size_t piece_list_t::linear_offset(const piece_node_t* start_node) const {
        size_t offset = 0;
        auto current_node = start_node;
        while (current_node != nullptr) {
            offset += current_node->length;
            current_node = current_node->prev;
        }
        return offset;
    }

    void piece_list_t::insert_after(piece_node_t* node, const piece_shared_ptr& piece) {
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

    void piece_list_t::insert_before(piece_node_t* node, const piece_shared_ptr& piece) {
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