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

    TEST_CASE("document_returns_text", "[document][!hide]") {
        core::caret cursor("caret");
        cursor.initialize(0, 0);

        core::document doc;
        doc.caret(&cursor);
        doc.page_size(25, 80);

        attr_t default_attr(1, 0, 0);

        std::string expected_text = "this is a test";

        for (auto c : expected_text) {
            doc.put(element_t{default_attr, static_cast<uint8_t>(c)});
        }

        auto spans = doc.line_at(0);
        REQUIRE(!spans.empty());
    }

}