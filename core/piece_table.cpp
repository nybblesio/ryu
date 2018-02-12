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

    void piece_t::copy_elements(attr_line_list& lines) {
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

    void piece_table_t::clear() {
        pieces.clear();
        changes.clear();
        original.clear();
        default_attr = {};
    }

    void piece_table_t::delete_at(
            const document_position_t& position,
            size_t length) {
        if (length == 0)
            return;
    }

    void piece_table_t::insert(
            const element_t& element,
            const document_position_t& position) {
        changes.elements.push_back(element);

        auto offset = position.offset();
        auto find_result = pieces.find_for_offset(offset);
        auto is_change_piece = find_result.data != nullptr ?
                               find_result.data->buffer->type == piece_table_buffer_t::changes :
                               false;
        switch (find_result.type) {
            case piece_find_result_t::first: {
                if (is_change_piece && find_result.data->end() + 1 == offset) {
                    find_result.data->length++;
                } else {
                    piece_t new_piece {offset, 1, &changes};
                    pieces.data.push_back(new_piece);
                }
                break;
            }
            case piece_find_result_t::final: {
                if (is_change_piece) {
                    find_result.data->length++;
                } else {
                    piece_t new_piece {offset, 1, &changes};
                    pieces.data.push_back(new_piece);
                }
                break;
            }
            case piece_find_result_t::medial: {
                //
                //
                // piece: 0................10
                //           ^
                //           3 index zero based
                //   left: 0..2
                //    new: 3
                //  right: 4..95

                auto new_change_offset = changes.size() - 1;
                auto linear_offset = pieces.linear_offset(*find_result.data);

                if (linear_offset == offset) {
                    find_result.data->length++;
                    break;
                }

                auto new_piece = piece_t {
                        static_cast<uint32_t>(new_change_offset),
                        1,
                        &changes};

                auto left_piece = find_result.data;
                auto right_piece = piece_t {
                        offset + 1,
                        left_piece->length - (offset + 1),
                        left_piece->buffer
                };

                left_piece->length = offset;

                if (find_result.at_back) {
                    pieces.data.push_back(new_piece);
                    pieces.data.push_back(right_piece);
                } else {
                    pieces.data.insert(find_result.index, new_piece);
                    pieces.data.insert(find_result.index, right_piece);
                }
                break;
            }
            default:
                // XXX: should never happen
                break;
        }
    }

    attr_line_list piece_table_t::sequence() {
        attr_line_list lines {};
        for (auto& piece : pieces.data) {
            piece.copy_elements(lines);
        }
        return lines;
    }

    void piece_table_t::load(const piece_table_buffer_t& buffer) {
        clear();
        original = buffer;

        piece_t first_piece {0, original.elements.size(), &original};
        pieces.data.push_back(first_piece);
    }

    ///////////////////////////////////////////////////////////////////////////

    size_t piece_list_t::total_length() const {
        size_t length = 0;
        for (auto& piece : data)
            length += piece.length;
        return length;
    }

    piece_find_result_t piece_list_t::find_for_offset(uint32_t offset) {
        piece_find_result_t result {};
        if (data.empty()) {
            return result;
        }

        if (offset == 0) {
            result.data = &data.front();
            result.type = piece_find_result_t::types::first;
        } else if (offset == total_length()) {
            result.data = &data.back();
            result.type = piece_find_result_t::types::final;
        } else {
            auto piece_offset = 0;
            auto piece_it = data.begin();
            while (piece_it != data.end()) {
                auto& current_piece = *piece_it;
                if (offset >= piece_offset
                &&  offset <= piece_offset + current_piece.length) {
                    break;
                }
                piece_offset += current_piece.length;
                ++piece_it;
            }

            if (piece_it != data.end()) {
                result.data = &(*piece_it);
                result.type = piece_find_result_t::types::medial;

                ++piece_it;

                result.index = piece_it;
                result.at_back = piece_it == data.end();
            }
        }

        return result;
    }

    size_t piece_list_t::linear_offset(const piece_t& piece) const {
        size_t offset = 0;
        auto it = data.cbegin();
        if (it == data.cend())
            return 0;
        while (it != data.cend()) {
            auto& current_piece = *it;
            offset += current_piece.length;
            if (current_piece == piece)
                break;
            ++it;
        }
        return offset;
    }

};