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

        uint32_t offset = 0;
        for (auto c : expected_text) {
            piece_table.insert_at(offset++, element_t{default_attr, (uint8_t) c});
        }

        REQUIRE(piece_table.original.empty());
        REQUIRE(piece_table.changes.size() == expected_text.length());
        REQUIRE(piece_table.pieces.size() == 1);
        REQUIRE(piece_table.pieces.undo_stack.size() == expected_text.length());

        SECTION("piece table returns valid sequence") {
            auto original_lines = piece_table.sequence();
            REQUIRE(original_lines.size() == 1);
            auto first_line = original_lines[0];
            REQUIRE(first_line.size() == 1);
            REQUIRE(first_line[0].text == expected_text);
        }

        SECTION("piece table undo reverts each edit") {
            piece_table.undo();
            piece_table.undo();
            piece_table.undo();
            piece_table.undo();
            piece_table.undo();
            piece_table.undo();
            piece_table.undo();

            REQUIRE(piece_table.pieces.undo_stack.size() == expected_text.length() - 7);
            REQUIRE(piece_table.pieces.redo_stack.size() == 7);

            auto original_lines = piece_table.sequence();
            REQUIRE(original_lines.size() == 1);
            auto first_line = original_lines[0];
            REQUIRE(first_line.size() == 1);
            REQUIRE(first_line[0].text == "A quick brown fox jumps over the");
        }
    }

    TEST_CASE("piece_table_with_empty_original_with_deletes", "[piece-table]") {
        piece_table_t piece_table;

        piece_table_buffer_t original {};
        piece_table.load(original);

        attr_t default_attr {1};
        std::string expected_text = "A quick brown fox jumps over the fence.";

        uint32_t offset = 0;
        for (auto c : expected_text) {
            piece_table.insert_at(offset++, element_t{default_attr, (uint8_t) c});
        }

        REQUIRE(piece_table.original.empty());
        REQUIRE(piece_table.changes.size() == expected_text.length());
        REQUIRE(piece_table.pieces.size() == 1);

        SECTION("sequence matches set up") {
            auto lines = piece_table.sequence();
            REQUIRE(lines.size() == 1);
            auto first_line = lines[0];
            REQUIRE(first_line.size() == 1);
            REQUIRE(first_line[0].text == expected_text);
        }

        SECTION("piece table shrinks piece when deleting at end of it") {
            piece_table.delete_at(39, 2);
            REQUIRE(piece_table.original.empty());
            REQUIRE(piece_table.changes.size() == expected_text.length());
            REQUIRE(piece_table.pieces.size() == 1);

            auto lines = piece_table.sequence();
            REQUIRE(lines.size() == 1);
            auto first_line = lines[0];
            REQUIRE(first_line.size() == 1);
            REQUIRE(first_line[0].text == "A quick brown fox jumps over the fenc");
        }

        SECTION("piece table shrinks piece when deleting at start of it") {
            piece_table.delete_at(0, 2);
            REQUIRE(piece_table.original.empty());
            REQUIRE(piece_table.changes.size() == expected_text.length());
            REQUIRE(piece_table.pieces.size() == 1);

            auto lines = piece_table.sequence();
            REQUIRE(lines.size() == 1);
            auto first_line = lines[0];
            REQUIRE(first_line.size() == 1);
            REQUIRE(first_line[0].text == "quick brown fox jumps over the fence.");
        }

        SECTION("piece table splits a piece when deleting within it") {
            piece_table.delete_at(13, 4);
            REQUIRE(piece_table.original.empty());
            REQUIRE(piece_table.changes.size() == expected_text.length());
            REQUIRE(piece_table.pieces.size() == 2);

            auto lines = piece_table.sequence();
            REQUIRE(lines.size() == 1);
            auto first_line = lines[0];
            REQUIRE(first_line.size() == 1);
            REQUIRE(first_line[0].text == "A quick brown jumps over the fence.");
        }
    }

    TEST_CASE("piece_table_with_empty_original_with_edits", "[piece-table]") {
        piece_table_t piece_table;

        piece_table_buffer_t original {};
        piece_table.load(original);

        attr_t default_attr {1};
        std::string expected_text = "A quick brown fox jumps over the fence.";

        uint32_t offset = 0;
        for (auto c : expected_text) {
            piece_table.insert_at(offset++, element_t{default_attr, (uint8_t) c});
        }

        REQUIRE(piece_table.original.empty());
        REQUIRE(piece_table.changes.size() == expected_text.length());
        REQUIRE(piece_table.pieces.size() == 1);

        SECTION("piece table sequence matches setup") {
            auto lines = piece_table.sequence();
            REQUIRE(lines.size() == 1);
            auto first_line = lines[0];
            REQUIRE(first_line.size() == 1);
            REQUIRE(first_line[0].text == expected_text);
        }

        SECTION("piece table inserts text within piece") {
            offset = 32;
            std::string inserted_text = " white ";
            for (auto c : inserted_text) {
                piece_table.insert_at(offset++, element_t{default_attr, (uint8_t) c});
            }

            REQUIRE(piece_table.original.empty());
            REQUIRE(piece_table.changes.size() == expected_text.length() + inserted_text.length());
            REQUIRE(piece_table.pieces.size() == 3);

            auto lines = piece_table.sequence();
            REQUIRE(lines.size() == 1);
            auto first_line = lines[0];
            REQUIRE(first_line.size() == 1);
            REQUIRE(first_line[0].text == "A quick brown fox jumps over the white fence.");
        }

        SECTION("piece table inserts text at the beginning of a piece") {
            offset = 32;
            std::string inserted_medial_text = " white ";
            for (auto c : inserted_medial_text) {
                piece_table.insert_at(offset++, element_t{default_attr, (uint8_t) c});
            }

            offset = 1;
            std::string inserted_initial_text = "n extremely ";
            for (auto c : inserted_initial_text) {
                piece_table.insert_at(offset++, element_t{default_attr, (uint8_t) c});
            }

            REQUIRE(piece_table.original.empty());
            REQUIRE(piece_table.changes.size() == expected_text.length() +
                                                          inserted_initial_text.length() +
                                                          inserted_medial_text.length());
            REQUIRE(piece_table.pieces.size() == 5);

            auto lines = piece_table.sequence();
            REQUIRE(lines.size() == 1);
            auto first_line = lines[0];
            REQUIRE(first_line.size() == 1);
            REQUIRE(first_line[0].text == "An extremely quick brown fox jumps over the white fence.");
        }

        SECTION("piece table deletes across pieces") {
            offset = 32;
            std::string inserted_text = " white ";
            for (auto c : inserted_text) {
                piece_table.insert_at(offset++, element_t{default_attr, (uint8_t) c});
            }

            REQUIRE(piece_table.original.empty());
            REQUIRE(piece_table.changes.size() == expected_text.length() + inserted_text.length());
            REQUIRE(piece_table.pieces.size() == 3);

            auto lines = piece_table.sequence();
            REQUIRE(lines.size() == 1);
            auto first_line = lines[0];
            REQUIRE(first_line.size() == 1);
            REQUIRE(first_line[0].text == "A quick brown fox jumps over the white fence.");

            piece_table.delete_at(29, 10);

            REQUIRE(piece_table.original.empty());
            REQUIRE(piece_table.changes.size() == expected_text.length() + inserted_text.length());
            REQUIRE(piece_table.pieces.size() == 2);

            auto updated_lines = piece_table.sequence();
            REQUIRE(updated_lines.size() == 1);
            auto updated_first_line = lines[0];
            REQUIRE(updated_first_line.size() == 1);
            REQUIRE(updated_first_line[0].text == "A quick brown fox jumps over fence.");
        }
    }

    TEST_CASE("piece_table_with_empty_original_with_different_attrs", "[piece-table]") {
        piece_table_t piece_table;

        piece_table_buffer_t original {};
        piece_table.load(original);

        attr_t default_attr {1};
        std::string expected_text = "A quick brown fox jumps over the fence.";

        uint8_t offset = 0;

        for (auto c : expected_text) {
            if (offset >= 2 && offset < 7) {
                default_attr.color = 4;
            } else {
                default_attr.color = 1;
            }
            piece_table.insert_at(offset++, element_t{default_attr, (uint8_t) c});
        }

        SECTION("piece table returns attributed spans matching setup") {
            REQUIRE(piece_table.original.empty());
            REQUIRE(piece_table.changes.size() == expected_text.length());
            REQUIRE(piece_table.pieces.size() == 1);

            auto lines = piece_table.sequence();
            REQUIRE(lines.size() == 1);
            auto first_line = lines[0];
            REQUIRE(first_line.size() == 3);
            REQUIRE(first_line[0].text == expected_text.substr(0, 2));
            REQUIRE(first_line[1].text == expected_text.substr(2, 5));
            REQUIRE(first_line[2].text == expected_text.substr(7));
        }

        SECTION("piece table returns attribute sections with medial insert") {
            offset = 32;
            std::string inserted_text = " white ";
            for (auto c : inserted_text) {
                piece_table.insert_at(offset++, element_t{default_attr, (uint8_t) c});
            }

            REQUIRE(piece_table.original.empty());
            REQUIRE(piece_table.changes.size() == expected_text.length() + inserted_text.length());
            REQUIRE(piece_table.pieces.size() == 3);

            auto lines = piece_table.sequence();
            REQUIRE(lines.size() == 1);
            auto first_line = lines[0];
            REQUIRE(first_line.size() == 3);
            REQUIRE(first_line[0].text == expected_text.substr(0, 2));
            REQUIRE(first_line[1].text == expected_text.substr(2, 5));
            REQUIRE(first_line[2].text == " brown fox jumps over the white fence.");
        }

        SECTION("piece table returns attributed spans with initial insert") {
            offset = 1;
            std::string inserted_text = "n extremely ";
            for (auto c : inserted_text) {
                piece_table.insert_at(offset++, element_t{default_attr, (uint8_t) c});
            }

            REQUIRE(piece_table.original.empty());
            REQUIRE(piece_table.changes.size() == expected_text.length()
                                                  + inserted_text.length());
            REQUIRE(piece_table.pieces.size() == 3);

            auto lines = piece_table.sequence();
            REQUIRE(lines.size() == 1);
            REQUIRE(lines.size() == 1);
            auto first_line = lines[0];
            REQUIRE(first_line.size() == 3);
            REQUIRE(first_line[0].text == "An extremely ");
            REQUIRE(first_line[1].text == "quick");
            REQUIRE(first_line[2].text == " brown fox jumps over the fence.");
        }
    }

}