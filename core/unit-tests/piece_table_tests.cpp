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
        piece_table_undo_manager undo_manager;

        piece_table piece_table;
        piece_table.undo_manager(&undo_manager);

        piece_table_buffer_t original {};
        piece_table.load(original);

        attr_t default_attr(1, 0, 0);
        std::string expected_text = "A quick brown fox jumps over the fence.";
        piece_table.insert_at(0, 0, element_list_t::from_string(default_attr, expected_text));

        REQUIRE(piece_table.original().empty());
        REQUIRE(piece_table.changes().size() == expected_text.length());
        REQUIRE(piece_table.pieces().size() == 1);
        REQUIRE(undo_manager.undo_depth() == 1);

        SECTION("piece table returns valid sequence") {
            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == expected_text);
        }

        SECTION("piece table undo reverts each edit") {
            piece_table.undo();

            REQUIRE(undo_manager.undo_depth() == 0);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.empty());
        }
    }

    TEST_CASE("piece_table_with_empty_original_with_deletes", "[piece-table]") {
        piece_table_undo_manager undo_manager;

        piece_table piece_table;
        piece_table.undo_manager(&undo_manager);

        piece_table_buffer_t original {};
        piece_table.load(original);

        attr_t default_attr(1, 0, 0);
        std::string expected_text = "A quick brown fox jumps over the fence.";
        piece_table.insert_at(0, 0, element_list_t::from_string(default_attr, expected_text));

        REQUIRE(piece_table.original().empty());
        REQUIRE(piece_table.changes().size() == expected_text.length());
        REQUIRE(piece_table.pieces().size() == 1);

        SECTION("sequence matches set up") {
            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == expected_text);
        }

        SECTION("piece table shrinks piece when deleting at end of it") {
            piece_table.delete_at(0, 39, 2);
            REQUIRE(piece_table.original().empty());
            REQUIRE(piece_table.changes().size() == expected_text.length());
            REQUIRE(piece_table.pieces().size() == 1);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == "A quick brown fox jumps over the fenc");
        }

        SECTION("piece table shrinks piece when deleting at start of it") {
            piece_table.delete_at(0, 0, 2);
            REQUIRE(piece_table.original().empty());
            REQUIRE(piece_table.changes().size() == expected_text.length());
            REQUIRE(piece_table.pieces().size() == 1);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == "quick brown fox jumps over the fence.");
        }

        SECTION("piece table splits a piece when deleting within it") {
            piece_table.delete_at(0, 13, 4);
            REQUIRE(piece_table.original().empty());
            REQUIRE(piece_table.changes().size() == expected_text.length());
            REQUIRE(piece_table.pieces().size() == 2);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == "A quick brown jumps over the fence.");
        }
    }

    TEST_CASE("piece_table_with_empty_original_with_edits", "[piece-table]") {
        piece_table_undo_manager undo_manager;

        piece_table piece_table;
        piece_table.undo_manager(&undo_manager);

        piece_table_buffer_t original {};
        piece_table.load(original);

        attr_t default_attr(1, 0, 0);
        std::string expected_text = "A quick brown fox jumps over the fence.";
        piece_table.insert_at(0, 0, element_list_t::from_string(default_attr, expected_text));

        REQUIRE(piece_table.original().empty());
        REQUIRE(piece_table.changes().size() == expected_text.length());
        REQUIRE(piece_table.pieces().size() == 1);

        SECTION("piece table sequence matches setup") {
            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == expected_text);
        }

        SECTION("piece table inserts text within piece") {
            std::string inserted_text = " white ";
            piece_table.insert_at(0, 32, element_list_t::from_string(default_attr, inserted_text));

            REQUIRE(piece_table.original().empty());
            REQUIRE(piece_table.changes().size() == expected_text.length() + inserted_text.length());
            REQUIRE(piece_table.pieces().size() == 3);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == "A quick brown fox jumps over the white fence.");
        }

        SECTION("piece table inserts text at the beginning of a piece") {
            std::string inserted_medial_text = " white ";
            piece_table.insert_at(0, 32, element_list_t::from_string(default_attr, inserted_medial_text));

            std::string inserted_initial_text = "n extremely ";
            piece_table.insert_at(0, 1, element_list_t::from_string(default_attr, inserted_initial_text));

            REQUIRE(piece_table.original().empty());
            REQUIRE(piece_table.changes().size() == expected_text.length() +
                                                          inserted_initial_text.length() +
                                                          inserted_medial_text.length());
            REQUIRE(piece_table.pieces().size() == 5);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == "An extremely quick brown fox jumps over the white fence.");
        }

        SECTION("piece table deletes across pieces") {
            std::string inserted_text = " white ";
            piece_table.insert_at(0, 32, element_list_t::from_string(default_attr, inserted_text));

            REQUIRE(piece_table.original().empty());
            REQUIRE(piece_table.changes().size() == expected_text.length() + inserted_text.length());
            REQUIRE(piece_table.pieces().size() == 3);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == "A quick brown fox jumps over the white fence.");

            SECTION("deleting across two pieces, where second is completely removed") {
                piece_table.delete_at(0, 29, 10);

                REQUIRE(piece_table.original().empty());
                REQUIRE(piece_table.changes().size() == expected_text.length() + inserted_text.length());
                REQUIRE(piece_table.pieces().size() == 2);

                auto updated_spans = piece_table.sequence(0);
                REQUIRE(updated_spans.size() == 1);
                REQUIRE(updated_spans[0].text == "A quick brown fox jumps over fence.");
            }

            SECTION("deleting across three pieces, where second is completely removed, and third is adjusted") {
                piece_table.delete_at(0, 29, 12);

                REQUIRE(piece_table.original().empty());
                REQUIRE(piece_table.changes().size() == expected_text.length() + inserted_text.length());
                REQUIRE(piece_table.pieces().size() == 2);

                auto updated_spans = piece_table.sequence(0);
                REQUIRE(updated_spans.size() == 1);
                REQUIRE(updated_spans[0].text == "A quick brown fox jumps over nce.");
            }

            SECTION("copy returns a sub-sequence of the piece table where the copy is within a single piece") {
                auto selection = piece_table.add_selection(selection_t::types::clipboard, 0, 2, 11);
                REQUIRE(piece_table.selections().size() == 1);
                auto copied_spans = piece_table.copy(selection);
                REQUIRE(copied_spans.size() == 1);
                REQUIRE(copied_spans.size() == 1);
                REQUIRE(copied_spans[0].text == "quick brown");
            }

            SECTION("copy returns a sub-sequence of the piece table where the copy spans multiple pieces") {
                auto selection = piece_table.add_selection(selection_t::types::clipboard, 0, 23, 20);
                REQUIRE(piece_table.selections().size() == 1);
                auto copied_spans = piece_table.copy(selection);
                REQUIRE(copied_spans.size() == 1);
                REQUIRE(copied_spans.size() == 1);
                REQUIRE(copied_spans[0].text == " over the white fenc");
            }

            SECTION("cut returns a sub-sequence of the piece table and deletes the range") {
                auto selection = piece_table.add_selection(selection_t::types::clipboard, 0, 23, 20);
                REQUIRE(piece_table.selections().size() == 1);
                auto copied_spans = piece_table.cut(selection);
                REQUIRE(copied_spans.size() == 1);
                REQUIRE(copied_spans[0].text == " over the white fenc");

                auto updated_spans = piece_table.sequence(0);
                REQUIRE(updated_spans.size() == 1);
                REQUIRE(updated_spans[0].text == "A quick brown fox jumpse.");
            }

            SECTION("paste into a range replaces the range of characters with the element_list") {
                auto selection = piece_table.add_selection(selection_t::types::clipboard, 0, 23, 20);
                REQUIRE(piece_table.selections().size() == 1);

                std::string to_paste = " fancy";
                auto elements = element_list_t::from_string(default_attr, to_paste);

                piece_table.paste(selection, elements);

                auto updated_spans = piece_table.sequence(0);
                REQUIRE(updated_spans.size() == 1);
                REQUIRE(updated_spans[0].text == "A quick brown fox jumps fancye.");
            }
        }
    }

    TEST_CASE("piece_table_with_empty_original_with_different_attrs", "[piece-table]") {
        piece_table_undo_manager undo_manager;

        piece_table piece_table;
        piece_table.undo_manager(&undo_manager);

        piece_table_buffer_t original {};
        piece_table.load(original);

        attr_t default_attr(1, 0, 0);
        std::string expected_text = "A quick brown fox jumps over the fence.";
        auto elements = element_list_t::from_string(default_attr, expected_text);

        uint8_t offset = 0;
        for (auto& e : elements) {
            if (offset >= 2 && offset < 7) {
                e.attr.color = 4;
            } else {
                e.attr.color = 1;
            }
            offset++;
        }

        piece_table.insert_at(0, 0, elements);

        SECTION("piece table returns attributed spans matching setup") {
            REQUIRE(piece_table.original().empty());
            REQUIRE(piece_table.changes().size() == expected_text.length());
            REQUIRE(piece_table.pieces().size() == 1);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 3);
            REQUIRE(spans[0].text == expected_text.substr(0, 2));
            REQUIRE(spans[1].text == expected_text.substr(2, 5));
            REQUIRE(spans[2].text == expected_text.substr(7));
        }

        SECTION("piece table returns attribute sections with medial insert") {
            std::string inserted_text = " white ";
            piece_table.insert_at(0, 32, element_list_t::from_string(default_attr, inserted_text));

            REQUIRE(piece_table.original().empty());
            REQUIRE(piece_table.changes().size() == expected_text.length() + inserted_text.length());
            REQUIRE(piece_table.pieces().size() == 3);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 3);
            REQUIRE(spans[0].text == expected_text.substr(0, 2));
            REQUIRE(spans[1].text == expected_text.substr(2, 5));
            REQUIRE(spans[2].text == " brown fox jumps over the white fence.");
        }

        SECTION("piece table returns attributed spans with initial insert") {
            std::string inserted_text = "n extremely ";
            piece_table.insert_at(0, 1, element_list_t::from_string(default_attr, inserted_text));

            REQUIRE(piece_table.original().empty());
            REQUIRE(piece_table.changes().size() == expected_text.length()
                                                  + inserted_text.length());
            REQUIRE(piece_table.pieces().size() == 3);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 3);
            REQUIRE(spans[0].text == "An extremely ");
            REQUIRE(spans[1].text == "quick");
            REQUIRE(spans[2].text == " brown fox jumps over the fence.");
        }
    }

}