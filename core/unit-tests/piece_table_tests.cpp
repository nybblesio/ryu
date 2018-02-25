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

    TEST_CASE("insert_at", "[piece-table-inserts]") {
        piece_table_undo_manager undo_manager;

        piece_table piece_table;
        piece_table.undo_manager(&undo_manager);

        piece_table_buffer_t original {};
        piece_table.load(original);

        attr_t default_attr(1, 0, 0);

        SECTION("first piece (insert)") {
            std::string expected_text = "A quick brown fox jumps over the fence.";
            piece_table.insert_at(0, 0, element_list_t::from_string(default_attr, expected_text));

            REQUIRE(piece_table.original().empty());
            REQUIRE(piece_table.changes().size() == expected_text.length());
            REQUIRE(piece_table.pieces().size() == 5);
            REQUIRE(undo_manager.undo_depth() == 1);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == expected_text);
        }

        SECTION("insert at end of existing piece (append)") {
            std::string expected_text1 = "A quick brown fox jumps over the fence.";
            piece_table.insert_at(
                0,
                0,
                element_list_t::from_string(default_attr, expected_text1));

            std::string expected_text2 = " I'm adding this onto the end of the existing piece.";
            piece_table.insert_at(
                0,
                static_cast<uint32_t>(expected_text1.length()),
                element_list_t::from_string(default_attr, expected_text2));

            REQUIRE(piece_table.original().empty());
            REQUIRE(piece_table.changes().size() == expected_text1.length() + expected_text2.length());
            REQUIRE(piece_table.pieces().size() == 5);
            REQUIRE(undo_manager.undo_depth() == 2);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == (expected_text1 + expected_text2));
        }

        SECTION("middle of piece (split)") {
            std::string expected_text = "A quick brown fox jumps over the fence.";
            piece_table.insert_at(0, 0, element_list_t::from_string(default_attr, expected_text));

            std::string inserted_text = " white";
            piece_table.insert_at(0, 32, element_list_t::from_string(default_attr, inserted_text));

            REQUIRE(piece_table.original().empty());
            REQUIRE(piece_table.changes().size() == expected_text.length() + inserted_text.length());
            REQUIRE(piece_table.pieces().size() == 7);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == "A quick brown fox jumps over the white fence.");
        }

        SECTION("one character in from start (split)") {
            std::string expected_text = "A quick brown fox jumps over the fence.";
            piece_table.insert_at(0, 0, element_list_t::from_string(default_attr, expected_text));

            std::string inserted_initial_text = "n extremely";
            piece_table.insert_at(0, 1, element_list_t::from_string(default_attr, inserted_initial_text));

            REQUIRE(piece_table.original().empty());
            REQUIRE(piece_table.changes().size() == expected_text.length() +
                                                    inserted_initial_text.length());
            REQUIRE(piece_table.pieces().size() == 7);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == "An extremely quick brown fox jumps over the fence.");
        }

        SECTION("multiple inserts within (split)") {
            std::string expected_text = "A quick brown fox jumps over the fence.";
            piece_table.insert_at(0, 0, element_list_t::from_string(default_attr, expected_text));

            std::string inserted_medial_text = " white";
            piece_table.insert_at(0, 32, element_list_t::from_string(default_attr, inserted_medial_text));

            std::string inserted_initial_text = "n extremely";
            piece_table.insert_at(0, 1, element_list_t::from_string(default_attr, inserted_initial_text));

            REQUIRE(piece_table.original().empty());
            REQUIRE(piece_table.changes().size() == expected_text.length() +
                                                    inserted_initial_text.length() +
                                                    inserted_medial_text.length());
            REQUIRE(piece_table.pieces().size() == 9);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == "An extremely quick brown fox jumps over the white fence.");
        }

        SECTION("non-adjacent pieces") {
            std::string expected_text1 = "Stuff at beginning...";
            piece_table.insert_at(0, 10, element_list_t::from_string(default_attr, expected_text1));

            std::string expected_text2 = "in the middle";
            piece_table.insert_at(0, 40, element_list_t::from_string(default_attr, expected_text2));

            std::string expected_text3 = "at the end.";
            piece_table.insert_at(0, 60, element_list_t::from_string(default_attr, expected_text3));

            REQUIRE(piece_table.original().empty());
            REQUIRE(piece_table.changes().size() ==
                    expected_text1.length() +
                    expected_text2.length() +
                    expected_text3.length());
            REQUIRE(piece_table.pieces().size() == 7);
            REQUIRE(undo_manager.undo_depth() == 3);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 3);
            REQUIRE(spans[0].text == "Stuff at beginning...");
            REQUIRE(spans[0].start_column == 10);
            REQUIRE(spans[1].text == "in the middle");
            REQUIRE(spans[1].start_column == 40);
            REQUIRE(spans[2].text == "at the end.");
            REQUIRE(spans[2].start_column == 60);
        }

        SECTION("just before a non-adjacent piece") {
            std::string expected_text1 = "Stuff at beginning...";
            piece_table.insert_at(0, 10, element_list_t::from_string(default_attr, expected_text1));

            std::string expected_text2 = "before";
            piece_table.insert_at(0, 10, element_list_t::from_string(default_attr, expected_text2));

            REQUIRE(piece_table.original().empty());
            REQUIRE(piece_table.changes().size() ==
                    expected_text1.length() +
                    expected_text2.length());
            REQUIRE(piece_table.pieces().size() == 6);
            REQUIRE(undo_manager.undo_depth() == 2);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == "beforeStuff at beginning...");
            REQUIRE(spans[0].start_column == 10);
        }
    }

    TEST_CASE("undo", "[piece-table-undo]") {
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
        REQUIRE(piece_table.pieces().size() == 5);
        REQUIRE(undo_manager.undo_depth() == 1);

        SECTION("revert single edit") {
            piece_table.undo();

            REQUIRE(undo_manager.undo_depth() == 0);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.empty());
        }

        SECTION("revert multiple edits") {
            piece_table.insert_at(0, 20, element_list_t::from_string(default_attr, "something silly"));
            auto spans1 = piece_table.sequence(0);
            REQUIRE(spans1.size() == 1);
            REQUIRE(spans1[0].text == "A quick brown fox jusomething sillymps over the fence.");

            piece_table.insert_at(0, 38, element_list_t::from_string(default_attr, " whoops!"));
            auto spans2 = piece_table.sequence(0);
            REQUIRE(spans2.size() == 1);
            REQUIRE(spans2[0].text == "A quick brown fox jusomething sillymps whoops! over the fence.");

            REQUIRE(undo_manager.undo_depth() == 3);
            REQUIRE(undo_manager.redo_depth() == 0);

            piece_table.undo();

            REQUIRE(undo_manager.undo_depth() == 2);
            REQUIRE(undo_manager.redo_depth() == 1);

            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == "A quick brown fox jusomething sillymps over the fence.");

            piece_table.undo();

            REQUIRE(undo_manager.undo_depth() == 1);
            REQUIRE(undo_manager.redo_depth() == 2);

            spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == "A quick brown fox jumps over the fence.");
        }

    }

    TEST_CASE("redo", "[piece-table-redo][!hide]") {
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
        REQUIRE(piece_table.pieces().size() == 5);
        REQUIRE(undo_manager.undo_depth() == 1);

        piece_table.insert_at(0, 20, element_list_t::from_string(default_attr, "something silly"));
        auto spans1 = piece_table.sequence(0);
        REQUIRE(spans1.size() == 1);
        REQUIRE(spans1[0].text == "A quick brown fox jusomething sillymps over the fence.");

        piece_table.insert_at(0, 38, element_list_t::from_string(default_attr, " whoops!"));
        auto spans2 = piece_table.sequence(0);
        REQUIRE(spans2.size() == 1);
        REQUIRE(spans2[0].text == "A quick brown fox jusomething sillymps whoops! over the fence.");

        REQUIRE(undo_manager.undo_depth() == 3);
        REQUIRE(undo_manager.redo_depth() == 0);

        piece_table.undo();

        REQUIRE(undo_manager.undo_depth() == 2);
        REQUIRE(undo_manager.redo_depth() == 1);

        auto spans = piece_table.sequence(0);
        REQUIRE(spans.size() == 1);
        REQUIRE(spans[0].text == "A quick brown fox jusomething sillymps over the fence.");

        piece_table.undo();

        REQUIRE(undo_manager.undo_depth() == 1);
        REQUIRE(undo_manager.redo_depth() == 2);

        spans = piece_table.sequence(0);
        REQUIRE(spans.size() == 1);
        REQUIRE(spans[0].text == "A quick brown fox jumps over the fence.");

        SECTION("redo multiple undos") {
//            piece_table.redo();
//
//            REQUIRE(undo_manager.undo_depth() == 2);
//            REQUIRE(undo_manager.redo_depth() == 1);
//
//            spans = piece_table.sequence(0);
//            REQUIRE(spans.size() == 1);
//            REQUIRE(spans[0].text == "A quick brown fox jusomething sillymps over the fence.");
//
//            piece_table.redo();
//
//            REQUIRE(undo_manager.undo_depth() == 3);
//            REQUIRE(undo_manager.redo_depth() == 0);
//
//            spans = piece_table.sequence(0);
//            REQUIRE(spans.size() == 1);
//            REQUIRE(spans[0].text == "A quick brown fox jumps over the fence.");
        }
    }

    TEST_CASE("delete_at", "[piece-table-deletes]") {
        piece_table_undo_manager undo_manager;

        piece_table piece_table;
        piece_table.undo_manager(&undo_manager);

        piece_table_buffer_t original {};
        piece_table.load(original);

        attr_t default_attr(1, 0, 0);
        std::string expected_text = "A quick brown fox jumps over the fence.";
        piece_table.insert_at(0, 0, element_list_t::from_string(default_attr, expected_text));

        std::string inserted_text1 = " white";
        piece_table.insert_at(0, 32, element_list_t::from_string(default_attr, inserted_text1));

        std::string inserted_text2 = "n extremely";
        piece_table.insert_at(0, 1, element_list_t::from_string(default_attr, inserted_text2));

        REQUIRE(piece_table.original().empty());
        REQUIRE(piece_table.changes().size() == expected_text.length()
                                                + inserted_text1.length()
                                                + inserted_text2.length());
        REQUIRE(piece_table.pieces().size() == 9);

        auto original_spans = piece_table.sequence(0);
        REQUIRE(original_spans.size() == 1);
        REQUIRE(original_spans[0].text == "An extremely quick brown fox jumps over the white fence.");

        SECTION("entire line") {
            piece_table.delete_at(0, 0, 56);
            REQUIRE(piece_table.pieces().size() == 4);
            auto spans = piece_table.sequence(0);
            REQUIRE(spans.empty());
        }

        SECTION("left edge") {
            piece_table.delete_at(0, 0, 13);
            REQUIRE(piece_table.pieces().size() == 7);
            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == "quick brown fox jumps over the white fence.");
        }

        SECTION("middle") {
            piece_table.delete_at(0, 19, 6);
            REQUIRE(piece_table.pieces().size() == 10);
            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == "An extremely quick fox jumps over the white fence.");
        }

        SECTION("right edge") {
            piece_table.delete_at(0, 34, 22);
            REQUIRE(piece_table.pieces().size() == 9);
            auto spans = piece_table.sequence(0);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == "An extremely quick brown fox jumps");
        }
    }

    TEST_CASE("sequences", "[piece-table-sequences]") {
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

        std::string inserted_text1 = " white";
        piece_table.insert_at(0, 32, element_list_t::from_string(default_attr, inserted_text1));

        std::string inserted_text2 = "n extremely";
        piece_table.insert_at(0, 1, element_list_t::from_string(default_attr, inserted_text2));

        REQUIRE(piece_table.original().empty());
        REQUIRE(piece_table.changes().size() == expected_text.length()
                                                + inserted_text1.length()
                                                + inserted_text2.length());
        REQUIRE(piece_table.pieces().size() == 9);

        auto original_spans = piece_table.sequence(0);
        REQUIRE(original_spans.size() == 3);
        REQUIRE(original_spans[0].text == "An extremely ");
        REQUIRE(original_spans[1].text == "quick");
        REQUIRE(original_spans[2].text == " brown fox jumps over the white fence.");

        SECTION("from left edge") {
            auto spans = piece_table.sub_sequence(0, 0, 25);
            REQUIRE(spans.size() == 3);
            REQUIRE(spans[0].text == "An extremely ");
            REQUIRE(spans[1].text == "quick");
            REQUIRE(spans[2].text == " brown ");
        }

        SECTION("from the middle") {
            auto spans = piece_table.sub_sequence(0, 13, 47);
            REQUIRE(spans.size() == 2);
            REQUIRE(spans[0].text == "quick");
            REQUIRE(spans[1].text == " brown fox jumps over the whi");
        }

        SECTION("from the right edge") {
            auto spans = piece_table.sub_sequence(0, 46, 57);
            REQUIRE(spans.size() == 1);
            REQUIRE(spans[0].text == "ite fence.");
        }
    }

    TEST_CASE("selections", "[piece-table-selections][!hide]") {
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
        REQUIRE(piece_table.pieces().size() == 5);

        auto original_spans = piece_table.sequence(0);
        REQUIRE(original_spans.size() == 1);
        REQUIRE(original_spans[0].text == expected_text);

        SECTION("copy") {
            SECTION("left edge") {
                auto selection = piece_table.add_selection(selection_t::types::clipboard, 0, 2, 11);
                REQUIRE(piece_table.selections().size() == 1);
                auto copied_spans = piece_table.copy(selection);
                REQUIRE(copied_spans.size() == 1);
                REQUIRE(copied_spans[0].text == "quick brown");
            }

            SECTION("middle") {
                auto selection = piece_table.add_selection(selection_t::types::clipboard, 0, 23, 20);
                REQUIRE(piece_table.selections().size() == 1);
                auto copied_spans = piece_table.copy(selection);
                REQUIRE(copied_spans.size() == 1);
                REQUIRE(copied_spans[0].text == " over the white fenc");
            }

            SECTION("right edge") {
                auto selection = piece_table.add_selection(selection_t::types::clipboard, 0, 23, 20);
                REQUIRE(piece_table.selections().size() == 1);
                auto copied_spans = piece_table.copy(selection);
                REQUIRE(copied_spans.size() == 1);
                REQUIRE(copied_spans[0].text == " over the white fenc");
            }
        }

        SECTION("cut") {
            auto selection = piece_table.add_selection(selection_t::types::clipboard, 0, 23, 20);
            REQUIRE(piece_table.selections().size() == 1);
            auto copied_spans = piece_table.cut(selection);
            REQUIRE(copied_spans.size() == 1);
            REQUIRE(copied_spans[0].text == " over the white fenc");

            auto updated_spans = piece_table.sequence(0);
            REQUIRE(updated_spans.size() == 1);
            REQUIRE(updated_spans[0].text == "A quick brown fox jumpse.");
        }

        SECTION("paste") {
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