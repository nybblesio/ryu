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

    TEST_CASE("document_inserts_at_middle_of_line", "[document]") {
        core::caret cursor("caret");
        cursor.initialize(0, 0);

        core::document doc;
        doc.caret(&cursor);
        doc.document_size(50, 128);
        doc.page_size(25, 80);

        attr_t default_attr(1, 0, 0);

        cursor.row(0);
        cursor.column(0);
        std::string expected_text1 = "this is a test that has a lot of text and something we can easily sub_sequence.";
        auto expected_length1 = static_cast<uint32_t>(expected_text1.length());
        doc.insert(element_list_t::from_string(default_attr, expected_text1));

        cursor.row(1);
        cursor.column(0);
        std::string expected_text2 = "second lines are always much more special than the first.";
        auto expected_length2 = static_cast<uint32_t>(expected_text2.length());
        doc.insert(element_list_t::from_string(default_attr, expected_text2));

        auto original_spans1 = doc.line_at(0, 0, expected_length1);
        REQUIRE(!original_spans1.empty());
        REQUIRE(original_spans1[0].text == expected_text1);

        auto original_spans2 = doc.line_at(1, 0, expected_length2);
        REQUIRE(!original_spans2.empty());
        REQUIRE(original_spans2[0].text == expected_text2);

        SECTION("insert text somewhere in the middle") {
            cursor.row(0);
            cursor.column(20);
            doc.insert(element_list_t::from_string(default_attr, "test "));

            REQUIRE(cursor.row() == 0);
            REQUIRE(cursor.column() == 25);

            auto spans = doc.line_at(0, 0, expected_length1 + 5);
            REQUIRE(!spans.empty());
            REQUIRE(spans[0].text == "this is a test that test has a lot of text and something we can easily sub_sequence.");
        }
    }

    TEST_CASE("document_inserts_at_head_of_line", "[document]") {
        core::caret cursor("caret");
        cursor.initialize(0, 0);

        core::document doc;
        doc.caret(&cursor);
        doc.document_size(50, 128);
        doc.page_size(25, 80);

        attr_t default_attr(1, 0, 0);

        cursor.row(0);
        cursor.column(0);
        std::string expected_text1 = "this is a test that has a lot of text and something we can easily sub_sequence.";
        auto expected_length1 = static_cast<uint32_t>(expected_text1.length());
        doc.insert(element_list_t::from_string(default_attr, expected_text1));

        cursor.row(1);
        cursor.column(0);
        std::string expected_text2 = "second lines are always much more special than the first.";
        auto expected_length2 = static_cast<uint32_t>(expected_text2.length());
        doc.insert(element_list_t::from_string(default_attr, expected_text2));

        auto original_spans1 = doc.line_at(0, 0, expected_length1);
        REQUIRE(!original_spans1.empty());
        REQUIRE(original_spans1[0].text == expected_text1);

        auto original_spans2 = doc.line_at(1, 0, expected_length2);
        REQUIRE(!original_spans2.empty());
        REQUIRE(original_spans2[0].text == expected_text2);

        SECTION("insert text at head of line 0") {
            cursor.row(0);
            cursor.column(0);
            doc.insert(element_list_t::from_string(default_attr, "test "));

            REQUIRE(cursor.row() == 0);
            REQUIRE(cursor.column() == 5);

            auto spans = doc.line_at(0, 0, expected_length1 + 5);
            REQUIRE(!spans.empty());
            REQUIRE(spans[0].text == "test " + expected_text1);
        }

        SECTION("insert text at head of line 1") {
            cursor.row(1);
            cursor.column(0);
            doc.insert(element_list_t::from_string(default_attr, "test "));

            REQUIRE(cursor.row() == 1);
            REQUIRE(cursor.column() == 5);

            auto spans = doc.line_at(1, 0, expected_length2 + 5);
            REQUIRE(!spans.empty());
            REQUIRE(spans[0].text == "test " + expected_text2);
        }
    }

    TEST_CASE("document_returns_text_by_line", "[document]") {
        core::caret cursor("caret");
        cursor.initialize(0, 0);

        core::document doc;
        doc.caret(&cursor);
        doc.document_size(50, 128);
        doc.page_size(25, 80);

        attr_t default_attr(1, 0, 0);

        cursor.row(0);
        cursor.column(0);
        std::string expected_text1 = "this is a test that has a lot of text and something we can easily sub_sequence.";
        auto expected_length1 = static_cast<uint32_t>(expected_text1.length());
        doc.insert(element_list_t::from_string(default_attr, expected_text1));

        cursor.row(1);
        cursor.column(0);
        std::string expected_text2 = "second lines are always much more special than the first.";
        auto expected_length2 = static_cast<uint32_t>(expected_text2.length());
        doc.insert(element_list_t::from_string(default_attr, expected_text2));

        auto original_spans1 = doc.line_at(0, 0, expected_length1);
        REQUIRE(!original_spans1.empty());
        REQUIRE(original_spans1[0].text == expected_text1);

        auto original_spans2 = doc.line_at(1, 0, expected_length2);
        REQUIRE(!original_spans2.empty());
        REQUIRE(original_spans2[0].text == expected_text2);

        SECTION("sub_sequence returns correct sub-selection: start at 4") {
            auto spans = doc.line_at(0, 4, expected_length1);
            REQUIRE(!spans.empty());
            REQUIRE(spans[0].text == expected_text1.substr(4));
        }

        SECTION("sub_sequence returns correct sub-selection: start at 8") {
            auto spans = doc.line_at(0, 8, expected_length1);
            REQUIRE(!spans.empty());
            REQUIRE(spans[0].text == expected_text1.substr(8));
        }

        SECTION("sub_sequence returns correct sub-selection: start at 20") {
            auto spans = doc.line_at(0, 20, expected_length1);
            REQUIRE(!spans.empty());
            REQUIRE(spans[0].text == expected_text1.substr(20));
        }

        SECTION("sub_sequence returns nothing if start beyond end") {
            auto spans = doc.line_at(0, expected_length1 + 1, expected_length1);
            REQUIRE(spans.empty());
        }
    }

}