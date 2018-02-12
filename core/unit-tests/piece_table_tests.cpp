//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <catch2.hpp>
#include <core/document.h>

namespace ryu::core::unit_tests {

    TEST_CASE("piece_table_with_empty_original_no_edits", "[piece-table]") {
        piece_table_t piece_table;

        piece_table_buffer_t original {};
        piece_table.load(original);

        attr_t default_attr {1};
        std::string expected_text = "A quick brown fox jumps over the fence.";

        uint16_t row = 0;
        uint8_t column = 0;

        for (auto c : expected_text) {
            piece_table.insert(
                    element_t{default_attr, (uint8_t) c},
                    document_position_t{row, column++, 25, 80});
        }

        REQUIRE(piece_table.original.empty());
        REQUIRE(piece_table.changes.size() == expected_text.length());
        REQUIRE(piece_table.pieces.size() == 1);

        auto original_lines = piece_table.sequence();
        REQUIRE(original_lines.size() == 1);
        auto& first_line = original_lines[0];
        REQUIRE(first_line.size() == 1);
        REQUIRE(first_line[0].text == expected_text);
    }

    TEST_CASE("piece_table_with_empty_original_with_deletes", "[piece-table]") {
        piece_table_t piece_table;

        piece_table_buffer_t original {};
        piece_table.load(original);

        attr_t default_attr {1};
        std::string expected_text = "A quick brown fox jumps over the fence.";

        uint16_t row = 0;
        uint8_t column = 0;

        for (auto c : expected_text) {
            piece_table.insert(
                    element_t{default_attr, (uint8_t) c},
                    document_position_t{row, column++, 25, 80});
        }

        REQUIRE(piece_table.original.empty());
        REQUIRE(piece_table.changes.size() == expected_text.length());
        REQUIRE(piece_table.pieces.size() == 1);

        auto original_lines = piece_table.sequence();
        REQUIRE(original_lines.size() == 1);
        auto& first_line = original_lines[0];
        REQUIRE(first_line.size() == 1);
        REQUIRE(first_line[0].text == expected_text);

        piece_table.delete_at(document_position_t{0, 39, 25, 80}, 2);
        REQUIRE(piece_table.original.empty());
        REQUIRE(piece_table.changes.size() == expected_text.length());
        REQUIRE(piece_table.pieces.size() == 1);

        auto deleted_lines1 = piece_table.sequence();
        REQUIRE(deleted_lines1.size() == 1);
        auto& first_deleted_line = deleted_lines1[0];
        REQUIRE(first_deleted_line.size() == 1);
        REQUIRE(first_deleted_line[0].text == "A quick brown fox jumps over the fenc");

        piece_table.delete_at(document_position_t{0, 0, 25, 80}, 2);
        REQUIRE(piece_table.original.empty());
        REQUIRE(piece_table.changes.size() == expected_text.length());
        REQUIRE(piece_table.pieces.size() == 1);

        auto deleted_lines2 = piece_table.sequence();
        REQUIRE(deleted_lines2.size() == 1);
        auto& second_deleted_line = deleted_lines2[0];
        REQUIRE(second_deleted_line.size() == 1);
        REQUIRE(second_deleted_line[0].text == "quick brown fox jumps over the fenc");
    }

    TEST_CASE("piece_table_with_empty_original_with_edits", "[piece-table]") {
        piece_table_t piece_table;

        piece_table_buffer_t original {};
        piece_table.load(original);

        attr_t default_attr {1};
        std::string expected_text = "A quick brown fox jumps over the fence.";

        uint16_t row = 0;
        uint8_t column = 0;

        for (auto c : expected_text) {
            piece_table.insert(
                    element_t{default_attr, (uint8_t) c},
                    document_position_t{row, column++, 25, 80});
        }

        REQUIRE(piece_table.original.empty());
        REQUIRE(piece_table.changes.size() == expected_text.length());
        REQUIRE(piece_table.pieces.size() == 1);

        auto original_lines = piece_table.sequence();
        REQUIRE(original_lines.size() == 1);
        auto& first_line = original_lines[0];
        REQUIRE(first_line.size() == 1);
        REQUIRE(first_line[0].text == expected_text);

        column = 32;
        std::string expected_edit1_text = " white ";
        for (auto c : expected_edit1_text) {
            piece_table.insert(
                    element_t{default_attr, (uint8_t) c},
                    document_position_t{row, column++, 25, 80});
        }

        REQUIRE(piece_table.original.empty());
        REQUIRE(piece_table.changes.size() == expected_text.length() + expected_edit1_text.length());
        REQUIRE(piece_table.pieces.size() == 3);

        auto edited_lines1 = piece_table.sequence();
        REQUIRE(edited_lines1.size() == 1);
        auto& first_edited_line = edited_lines1[0];
        REQUIRE(first_edited_line.size() == 1);
        REQUIRE(first_edited_line[0].text == "A quick brown fox jumps over the white fence.");

        column = 1;
        std::string expected_edit2_text = "n extremely ";
        for (auto c : expected_edit2_text) {
            piece_table.insert(
                    element_t{default_attr, (uint8_t) c},
                    document_position_t{row, column++, 25, 80});
        }

        REQUIRE(piece_table.original.empty());
        REQUIRE(piece_table.changes.size() == expected_text.length()
                                              + expected_edit1_text.length()
                                              + expected_edit2_text.length());
        REQUIRE(piece_table.pieces.size() == 5);

        auto edited_lines2 = piece_table.sequence();
        REQUIRE(edited_lines2.size() == 1);
        auto& first_edited_line2 = edited_lines2[0];
        REQUIRE(first_edited_line2.size() == 1);
        REQUIRE(first_edited_line2[0].text == "An extremely quick brown fox jumps over the white fence.");
    }

    TEST_CASE("piece_table_with_empty_original_with_different_attrs", "[piece-table]") {
        piece_table_t piece_table;

        piece_table_buffer_t original {};
        piece_table.load(original);

        attr_t default_attr {1};
        std::string expected_text = "A quick brown fox jumps over the fence.";

        uint16_t row = 0;
        uint8_t column = 0;

        for (auto c : expected_text) {
            if (column >= 2 && column < 7) {
                default_attr.color = 4;
            } else {
                default_attr.color = 1;
            }
            piece_table.insert(
                    element_t{default_attr, (uint8_t) c},
                    document_position_t{row, column++, 25, 80});
        }

        REQUIRE(piece_table.original.empty());
        REQUIRE(piece_table.changes.size() == expected_text.length());
        REQUIRE(piece_table.pieces.size() == 1);

        auto original_lines = piece_table.sequence();
        REQUIRE(original_lines.size() == 1);
        auto& original_first_line = original_lines[0];
        REQUIRE(original_first_line.size() == 3);
        REQUIRE(original_first_line[0].text == expected_text.substr(0, 2));
        REQUIRE(original_first_line[1].text == expected_text.substr(2, 5));
        REQUIRE(original_first_line[2].text == expected_text.substr(7));

        column = 32;
        std::string expected_edit1_text = " white ";
        for (auto c : expected_edit1_text) {
            piece_table.insert(
                    element_t{default_attr, (uint8_t) c},
                    document_position_t{row, column++, 25, 80});
        }

        REQUIRE(piece_table.original.empty());
        REQUIRE(piece_table.changes.size() == expected_text.length() + expected_edit1_text.length());
        REQUIRE(piece_table.pieces.size() == 3);

        auto edited_lines1 = piece_table.sequence();
        REQUIRE(edited_lines1.size() == 1);
        auto& first_edited_lines1 = edited_lines1[0];
        REQUIRE(first_edited_lines1.size() == 3);
        REQUIRE(first_edited_lines1[0].text == expected_text.substr(0, 2));
        REQUIRE(first_edited_lines1[1].text == expected_text.substr(2, 5));
        REQUIRE(first_edited_lines1[2].text == " brown fox jumps over the white fence.");

        column = 1;
        std::string expected_edit2_text = "n extremely ";
        for (auto c : expected_edit2_text) {
            piece_table.insert(
                    element_t{default_attr, (uint8_t) c},
                    document_position_t{row, column++, 25, 80});
        }

        REQUIRE(piece_table.original.empty());
        REQUIRE(piece_table.changes.size() == expected_text.length()
                                              + expected_edit1_text.length()
                                              + expected_edit2_text.length());
        REQUIRE(piece_table.pieces.size() == 5);

        auto edited_lines2 = piece_table.sequence();
        REQUIRE(edited_lines2.size() == 1);
        REQUIRE(edited_lines2.size() == 1);
        auto& first_edited_lines2 = edited_lines2[0];
        REQUIRE(first_edited_lines2.size() == 3);
        REQUIRE(first_edited_lines2[0].text == "An extremely ");
        REQUIRE(first_edited_lines2[1].text == "quick");
        REQUIRE(first_edited_lines2[2].text == " brown fox jumps over the white fence.");
    }

}