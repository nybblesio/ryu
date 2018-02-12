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
        REQUIRE(piece_table.pieces.size() == 2);

        auto original_spans = piece_table.sequence();
        REQUIRE(original_spans.size() == 1);
        REQUIRE(original_spans[0].text == expected_text);
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
        REQUIRE(piece_table.pieces.size() == 2);

        auto original_spans = piece_table.sequence();
        REQUIRE(original_spans.size() == 1);
        REQUIRE(original_spans[0].text == expected_text);

        column = 32;
        std::string expected_edit1_text = " white ";
        for (auto c : expected_edit1_text) {
            piece_table.insert(
                    element_t{default_attr, (uint8_t) c},
                    document_position_t{row, column++, 25, 80});
        }

        REQUIRE(piece_table.original.empty());
        REQUIRE(piece_table.changes.size() == expected_text.length() + expected_edit1_text.length());
        REQUIRE(piece_table.pieces.size() == 4);

        auto edited_spans1 = piece_table.sequence();
        REQUIRE(edited_spans1.size() == 3);
        REQUIRE(edited_spans1[0].text == "A quick brown fox jumps over the");
        REQUIRE(edited_spans1[1].text == " white ");
        REQUIRE(edited_spans1[2].text == "fence.");

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
        REQUIRE(piece_table.pieces.size() == 6);

        auto edited_spans2 = piece_table.sequence();
        REQUIRE(edited_spans2.size() == 5);
        REQUIRE(edited_spans2[0].text == "A");
        REQUIRE(edited_spans2[1].text == "n extremely ");
        REQUIRE(edited_spans2[2].text == "quick brown fox jumps over the");
        REQUIRE(edited_spans2[3].text == " white ");
        REQUIRE(edited_spans2[4].text == "fence.");
    }

}